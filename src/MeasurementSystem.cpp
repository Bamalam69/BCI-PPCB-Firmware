// #include "MeasurementSystem.h"
// // #include "attachInterruptEx.h"
// #include <Arduino.h>
// #include <functional>
// #include "SampleAccumulator.h"

// // SampleData MeasurementSystem::sampleData = {.data = {0}};
// SampleAccumulator MeasurementSystem::sampleAccumulator;

// constexpr uint8_t MeasurementSystem::NUM_ADCS;
// uint8_t MeasurementSystem::_numAdcs;
// ADC *MeasurementSystem::_adcs[NUM_ADCS];
// bool MeasurementSystem::sampleReady;

// constexpr uint8_t MeasurementSystem::LEN;
// BLEConnection *MeasurementSystem::conn;
// uint8_t MeasurementSystem::_message[MeasurementSystem::LEN];
// BLEDis MeasurementSystem::bledis;
// BLEService MeasurementSystem::eegService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
// BLECharacteristic MeasurementSystem::eegCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLERead | BLENotify);

// bool MeasurementSystem::_block;
// int MeasurementSystem::_start, MeasurementSystem::_count;
// uint32_t MeasurementSystem::_timestamp, MeasurementSystem::_checkpoint, MeasurementSystem::_quotient, MeasurementSystem::_interval;

// MeasurementSystem::MeasurementSystem() {}

// void MeasurementSystem::setup(ADCInit *adcInits, const uint8_t numAdcs)
// {
    

//     _numAdcs = numAdcs;
//     for (uint8_t i = 0; i < numAdcs; i++)
//     {
//         _adcs[i] = new ADC();
//         _adcs[i]->setup(adcInits[i]);
//     }

//     Serial.println("Finished setting up adc.");
//     // Serial.println("Setting up Bluetooth...");

//     // // /* Setup bluetooth: */
//     // Bluefruit.autoConnLed(true);

//     // // // Note: All config***() function must be called before begin()
//     // Bluefruit.configPrphConn(LEN + 3, LEN, 20, 2);

//     // Bluefruit.begin();
//     // Bluefruit.setTxPower(8); // Check bluefruit.h for supported values
//     // Bluefruit.Periph.setConnectCallback(connect_callback);
//     // Bluefruit.Periph.setDisconnectCallback(disconnect_callback);
//     // // Bluefruit.Periph.setConnInterval(6, 12);

//     // // Configure and Start Device Information Service
//     // bledis.setManufacturer("Adafruit Industries");
//     // bledis.setModel("Bluefruit Feather52");
//     // bledis.begin();

//     // // Setup eeg service
//     // Serial.println("Setting up EEG service...");
//     // eegService.begin();

//     // eegCharacteristic.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
//     // eegCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
//     // // eegCharacteristic.setFixedLen(100);
//     // eegCharacteristic.setMaxLen(150);
//     // eegCharacteristic.begin();
//     // eegCharacteristic.write("Hello World!", 13);
//     // // Serial.println("HERE #2");

//     // // Set up and start advertising
//     // Serial.println("Starting advertising...");
//     // Serial.println("STARTING TO ADVERTISE!!");

//     // // Include bleuart 128-bit uuid
//     // Bluefruit.Advertising.addService(eegService);

//     // // There is no room for Name in Advertising packet
//     // // Use Scan response for Name
//     // Bluefruit.ScanResponse.addName();

//     // /* Start Advertising
//     //  * - Enable auto advertising if disconnected
//     //  * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
//     //  * - Timeout for fast mode is 30 seconds
//     //  * - Start(timeout) with timeout = 0 will advertise forever (until connected)
//     //  *
//     //  * For recommended advertising interval
//     //  * https://developer.apple.com/library/content/qa/qa1931/_index.html
//     //  */
//     // Bluefruit.Advertising.restartOnDisconnect(true);
//     // Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
//     // Bluefruit.Advertising.setFastTimeout(60);   // number of seconds in fast mode
//     // Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds

//     // Serial.println("HERE #3");
// }

// void MeasurementSystem::start()
// {
//     Serial.println("Starting ADCs!");
//     for (int8_t i = _numAdcs - 1; i >= 0; i--)
//     {
//         Serial.println(i);
//         digitalWrite(_adcs[i]->getStartPin(), LOW);
//         delay(10);
//         digitalWrite(_adcs[i]->getStartPin(), HIGH);
//     }
//     Serial.println("Starting ADCs (2)!");
//     for (int8_t i = _numAdcs - 1; i >= 0; i--)
//     {
//         Serial.println(i);
//         ad7779_set_spi_op_mode(_adcs[i]->_getRaw(), ad7779_spi_op_mode::AD7779_SD_CONV);
//     }

//     attachInterrupt(digitalPinToInterrupt(_adcs[0]->getDrdyPin()), onDataReadyStatic, FALLING);
//     Serial.println("Started ADCs!");
// }
