#include <Arduino.h>
#include <ADC.h>
#include <SPI.h>
#include "SampleAccumulator.h"
#include <bluefruit.h>

ADCInit adcInit1 = {
    .startPin = PIN_A1,
    .drdyPin = PIN_A2,
    .resetPin = PIN_A0,
    .chipSelectPin = 13,
};

ADCInit adcInit2 = {
    .startPin = PIN_A4,
    .drdyPin = PIN_A5,
    .resetPin = PIN_A3,
    .chipSelectPin = 12,
};

ADCInit adcInit3 = {
    .startPin = PIN_SERIAL1_TX,
    .drdyPin = 9,
    .resetPin = PIN_SERIAL1_RX,
    .chipSelectPin = 11,
};

ADCInit adcInit4 = {
    .startPin = 5,
    .drdyPin = PIN_WIRE_SCL,
    .resetPin = 6,
    .chipSelectPin = 10,
};

ADCInit adcInits[] = {adcInit1, adcInit2, adcInit3, adcInit4};

const uint8_t numAdcs = 4;
ADC _adcs[] = {
    ADC(),
    ADC(),
    ADC(),
    ADC(),
};

// Define the synchronization sequences:
const char START_SEQ[] = {0xFF, 0xFE, 0xFD, 0xFC};
const char END_SEQ[] = {0xFC, 0xFD, 0xFE, 0xFF};

SampleAccumulator sampleAccumulator;

const uint8_t RING_BUFFER_SIZE = 100;
volatile int32_t ringBuffer[RING_BUFFER_SIZE][numAdcs * 8 + 1];
volatile uint8_t readIndex = 0;
volatile uint8_t writeIndex = 0;

BLEDis bledis;
BLEUart bleuart;
BLEConnection *conn;

const int LEN = sizeof(Sample);

void onDataReady();
void startAdv();
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);

void setup()
{
    delay(7500);
    dwt_enable();
    Serial.begin(115200);
    while (!Serial)
        delay(10);

        // // Setup bluetooth:
        // Bluefruit.autoConnLed(true);

        // // Note: All config***() function must be called before begin()
        // Bluefruit.configPrphConn(LEN + 3, LEN, 20, 2);

        // Bluefruit.begin();
        // Bluefruit.setTxPower(8); // Check bluefruit.h for supported values
        // Bluefruit.Periph.setConnectCallback(connect_callback);
        // Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
        // Bluefruit.Periph.setConnInterval(6, 12);

        // // Configure and Start Device Information Service
        // bledis.setManufacturer("Adafruit Industries");
        // bledis.setModel("Bluefruit Feather52");
        // bledis.begin();

        // // Configure and Start BLE Uart Service
        // bleuart.begin();

        // // Set up and start advertising
        // startAdv();

#ifdef DEBUG
    Serial.println("Setting up ADCs...");
#endif

    // Give ADCs their pins:
    for (uint8_t i = 0; i < numAdcs; i++)
    {
        _adcs[i].setup(adcInits[i]);
    }

#ifdef DEBUG
    Serial.println("Starting sampling...");
#endif

    for (int8_t i = numAdcs - 1; i >= 0; i--) {
        digitalWrite(_adcs[i].getStartPin(), LOW);
        delay(10);
        digitalWrite(_adcs[i].getStartPin(), HIGH);
    }

    // for (int8_t i = numAdcs - 1; i >= 0; i--) {
    // }

    for (int8_t i = numAdcs - 1; i >= 0; i--) {
        _adcs[i].enableSPIControlMode();
    }

    // Start allowing interrupts...
    _adcs[0].setupDataISR(onDataReady);

#ifdef DEBUG
    Serial.println("Sampling started!");
#endif
}

void loop()
{
    while (readIndex != writeIndex)
    {
        noInterrupts();
        Sample sample;
        sample.timestamp = ringBuffer[readIndex][0];
        for (uint8_t i = 0; i < numAdcs * 8; i++)
        {
            sample.data[i] = ringBuffer[readIndex][i + 1];
        }
        readIndex = (readIndex + 1) % RING_BUFFER_SIZE;
        interrupts();

        // bleuart.write((const uint8_t *)&sample, sizeof(sample));
        
        // Fill the struct with the last sample read...

        // Send the synchronization sequence and the actual sample data:
        Serial.write(START_SEQ, sizeof(START_SEQ));
        Serial.write((uint8_t *)&sample, sizeof(sample));
        Serial.write(END_SEQ, sizeof(END_SEQ));

    }
}

void onDataReady()
{
    Sample sampleData;
    // Get the timestamp of sample read:
    sampleData.timestamp = micros();

    for (uint8_t adcIndex = 0; adcIndex < numAdcs; adcIndex++)
    {
        // Grab sample from ADC:
        ADCSample adcData = _adcs[adcIndex].readSample();
        memcpy(&sampleData.data[adcIndex * 8], adcData.data, 
            sizeof(adcData.data));
    }

    // Put the sample data in the ring buffer.
    // Can't use memcpy cos of volatility...
    ringBuffer[writeIndex][0] = sampleData.timestamp;
    for (uint8_t i = 0; i < numAdcs * 8; i++)
    {
        ringBuffer[writeIndex][i + 1] = sampleData.data[i];
    }
    writeIndex = (writeIndex + 1) % RING_BUFFER_SIZE;
}

void startAdv(void)
{
    // Include bleuart 128-bit uuid
    Bluefruit.Advertising.addService(bleuart);

    // There is no room for Name in Advertising packet
    // Use Scan response for Name
    Bluefruit.ScanResponse.addName();

    /* Start Advertising
     * - Enable auto advertising if disconnected
     * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     * - Timeout for fast mode is 30 seconds
     * - Start(timeout) with timeout = 0 will advertise forever (until connected)
     *
     * For recommended advertising interval
     * https://developer.apple.com/library/content/qa/qa1931/_index.html
     */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(60);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
}

void connect_callback(uint16_t conn_handle)
{
    conn = Bluefruit.Connection(conn_handle);

    char central_name[32] = {0};
    conn->getPeerName(central_name, sizeof(central_name));

    Serial.print("Connected to ");
    Serial.println(central_name);

    // request PHY changed to 2MB
    Serial.println("Request to change PHY");
    conn->requestPHY(BLE_GAP_PHY_2MBPS);

    // request connection interval of 7.5 ms
    Serial.println("Request to change connection params");
    conn->requestConnectionParameter(6, 0, 10); // in unit of 1.25

    // delay a bit for all the request to complete
    delay(1000);

    for (int8_t i = numAdcs - 1; i >= 0; i--)
    {
        digitalWrite(_adcs[i].getStartPin(), LOW);
        delay(10);
        digitalWrite(_adcs[i].getStartPin(), HIGH);
    }
    for (int8_t i = numAdcs - 1; i >= 0; i--)
    {
        _adcs[i].enableSPIControlMode();
    }

    // Start allowing interrupts...
    _adcs[0].setupDataISR(onDataReady);
}

/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void)conn_handle;
    (void)reason;

    Serial.println();
    Serial.print("Disconnected, reason = 0x");
    Serial.println(reason, HEX);
}