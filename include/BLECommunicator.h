#ifndef BLE_COMMUNICATOR_H_
#define BLE_COMMUNICATOR_H_

#include <Arduino.h>
#include <bluefruit.h>
#include "SampleAccumulator.h"

class BLECommunicator {
public:
    BLECommunicator() {
        _instance = this;
    };
    
    void setup();
    void SendSample(SampleAccumulator& sampleAccumulator);
    bool isConnected() const;

protected:
    static void connect_callback(uint16_t conn_handle);
    static void disconnect_callback(uint16_t conn_handle, uint8_t reason);

private:
    BLEService _service = BLEService("19B10000-E8F2-537E-4F6C-D104768A1214");
    BLECharacteristic _characteristic = BLECharacteristic("19B10001-E8F2-537E-4F6C-D104768A1214", BLERead | BLENotify, sizeof(double) * (32 + 1));
    BLEDis _dis;
    BLEConnection* _connection = nullptr;

    static BLECommunicator* _instance;
};

#endif