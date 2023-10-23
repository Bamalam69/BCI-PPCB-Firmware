// #ifndef MEASUREMENT_SYSTEM_H_
// #define MEASUREMENT_SYSTEM_H_

// #include <Arduino.h>
// #include <ADC.h>
// #include <bluefruit.h>
// #include <queue>
// #include "SampleAccumulator.h"
// #include "SampleData.h"

// #define NUM_CHANNELS_PER_ADC 8

// class MeasurementSystem
// {
// public:

//     MeasurementSystem();
//     MeasurementSystem(const MeasurementSystem &) = delete;
//     MeasurementSystem &operator=(const MeasurementSystem &) = delete;

//     void setup(ADCInit *adcInits, const uint8_t numAdcs = 4);
//     void start();

//     bool sampleAvailable() { return sampleAccumulator.hasSamples(); }
//     bool connected() { return conn && conn->connected(); }

//     void sendSample(SampleData &sample)
//     {
//         // if (conn && conn->connected())
//         // {
//         // Convert into byte array for sending:
//         double *byteArray = (double *)sample.data;
//         for (uint8_t i = 0; i < _numAdcs * 8; i++)
//         {
//             Serial.print(byteArray[i], 24);
//             if (i != _numAdcs * 8 - 1)
//                 Serial.print(",");
//         }
//         Serial.print("\n");

//         // eegCharacteristic.notify((void *)byteArray, sizeof(sample->data));
//         // eegCharacteristic.write("TESTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT");
//         // }
//     }

//     std::queue<SampleData> getSamples()
//     {
//         sampleReady = false;
//         // const uint8_t numChannelsPerADC = 8;
//         // const uint8_t numChannels = numChannelsPerADC * _numAdcs;
//         // double channelData[numChannels] = {0};
//         // // uint8_t channelInfo[numChannels] = {0};
//         // // Serial.print("SAMPLE!");
//         // for (uint8_t adcIndex = 0; adcIndex < _numAdcs; adcIndex++)
//         // {
//         //     ad7779_dev *dev = _adcs[adcIndex]->_getRaw();
//         //     for (uint8_t channel = 0; channel < numChannelsPerADC; channel++)
//         //     {
//         //         const uint8_t bytes_number = 4;
//         //         uint8_t data[] = {0x80, 0x00, 0x00, 0x00};
//         //         no_os_spi_write_and_read(dev->spi_desc, data, bytes_number, false);

//         //         uint8_t header = data[0];
//         //         int32_t value = (data[1] << 16) | (data[2] << 8) | data[3];

//         //         value <<= 8;
//         //         value >>= 8;

//         //         // Perform voltage conversion:
//         //         double voltage = (value * 3) / pow(2, 24);

//         //         // hadError = header & 0x80;

//         //         // uint8_t channelId = (header >> 4) & 0x7;
//         //         channelData[channel * (adcIndex + 1)] = voltage;

//         //         // Serial.print(channelId);
//         //         // Serial.print(":");
//         //         Serial.print(voltage, 24);
//         //         if (channel != numChannelsPerADC - 1)
//         //             Serial.print(",");
//         //         // channelInfo[channel * adcIndex] = channelId;
//         //     }
//         //     // break;
//         // }
//         // Serial.print("\n");
//         // SampleData *sampleData = (SampleData *)malloc(sizeof(SampleData));
//         // memcpy(sampleData->data, channelData, sizeof(channelData));
//         // return sampleData;
        
//         return sampleAccumulator.getSamples();
//     }

//     static void onDataReadyStatic()
//     {
//         // Serial.println("SAMPLE RECIEVED!");
//         // Serial.print("Current count of samples: ");
//         // Serial.println(sampleAccumulator.hasSamples());
//         sampleReady = true;
//         const uint8_t numChannelsPerADC = 8;
//         const uint8_t numChannels = numChannelsPerADC * _numAdcs;
//         SampleData sampleData;
//         // uint8_t channelInfo[numChannels] = {0};
//         // Serial.print("SAMPLE!");
//         for (uint8_t adcIndex = 0; adcIndex < _numAdcs; adcIndex++)
//         {
//             ad7779_dev *dev = _adcs[adcIndex]->_getRaw();
//             for (uint8_t channel = 0; channel < numChannelsPerADC; channel++)
//             {
//                 const uint8_t bytes_number = 4;
//                 uint8_t data[] = {0x80, 0x00, 0x00, 0x00};
//                 no_os_spi_write_and_read(dev->spi_desc, data, bytes_number, false);

//                 uint8_t header = data[0];
//                 int32_t value = (data[1] << 16) | (data[2] << 8) | data[3];

//                 value <<= 8;
//                 value >>= 8;

//                 // Perform voltage conversion:
//                 double voltage = (value * 3) / pow(2, 24);

//                 // hadError = header & 0x80;

//                 // uint8_t channelId = (header >> 4) & 0x7;
//                 sampleData.data[channel * (adcIndex + 1)] = voltage;

//                 // Serial.print(channelId);
//                 // Serial.print(":");
//                 // Serial.print(voltage, 24);
//                 // if (channel != numChannelsPerADC - 1)
//                     // Serial.print(",");
//                 // channelInfo[channel * adcIndex] = channelId;
//             }
//             // break;
//         }
//         // Serial.print("\n");

//         sampleAccumulator.addSample(sampleData);
//         // memccpy(sampleData.data, channelData, sizeof(channelData), sizeof(channelData));

//         // Store sample in accumulator:
//         // sampleAccumulator.addSample(sampleData);

//         // eegCharacteristic.notify("Sample received!");
//     };

//     void startAdv(void)
//     {
//         Serial.println("STARTING TO ADVERTISE!!");

//         // Include bleuart 128-bit uuid
//         Bluefruit.Advertising.addService(bleuart);

//         // There is no room for Name in Advertising packet
//         // Use Scan response for Name
//         Bluefruit.ScanResponse.addName();

//         /* Start Advertising
//          * - Enable auto advertising if disconnected
//          * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
//          * - Timeout for fast mode is 30 seconds
//          * - Start(timeout) with timeout = 0 will advertise forever (until connected)
//          *
//          * For recommended advertising interval
//          * https://developer.apple.com/library/content/qa/qa1931/_index.html
//          */
//         Bluefruit.Advertising.restartOnDisconnect(true);
//         Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
//         Bluefruit.Advertising.setFastTimeout(60);   // number of seconds in fast mode
//         Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds
//     }

//     static void connect_callback(uint16_t conn_handle)
//     {
//         conn = Bluefruit.Connection(conn_handle);

//         char central_name[32] = {0};
//         conn->getPeerName(central_name, sizeof(central_name));

//         conn->requestPHY(BLE_GAP_PHY_2MBPS); // 2MPS
//         conn->requestConnectionParameter(6, 0, 10);

//         Serial.print("Connected to ");
//         Serial.println(central_name);

//         delay(1000);
//     }

//     /**
//      * Callback invoked when a connection is dropped
//      * @param conn_handle connection where this event happens
//      * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
//      */
//     static void disconnect_callback(uint16_t conn_handle, uint8_t reason)
//     {
//         (void)conn_handle;
//         (void)reason;

//         Serial.println();
//         Serial.print("Disconnected, reason = 0x");
//         Serial.println(reason, HEX);
//     }

// private:
//     static SampleAccumulator sampleAccumulator;
//     // double _sample[8]; // For holding a single sample

//     // For holding samples made whilst grabbing them in main: (In case the interrupt fires whilst we're grabbing samples)
//     // static SampleAccumulator _internalAccumulator;

//     /* ADC Handling: */
//     constexpr static uint8_t NUM_ADCS = 4;
//     static uint8_t _numAdcs;
//     static ADC *_adcs[NUM_ADCS];
//     static bool sampleReady;

//     /* Bluetooth: */
//     constexpr static uint8_t LEN = 100;
//     static uint8_t _message[LEN];
//     static BLEDis bledis;
//     static BLEUart bleuart;
//     static BLEConnection *conn;
//     static bool _block;
//     static int _start, _count;
//     static uint32_t _timestamp, _checkpoint, _quotient, _interval;
//     static BLEService eegService;
//     static BLECharacteristic eegCharacteristic;
// };

// #endif