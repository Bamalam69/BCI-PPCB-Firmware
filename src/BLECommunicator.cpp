#include "BLECommunicator.h"

BLECommunicator* BLECommunicator::_instance = nullptr;

void BLECommunicator::setup()
{
    Serial.println("Initializing BLE...");

    Bluefruit.configPrphConn(sizeof(double) * (32 + 1) + 3, sizeof(double) * (32 + 1), 20, 2);

    Bluefruit.begin();
    Bluefruit.setName("EEG Monitor");
    Bluefruit.setTxPower(8);
    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

    // Configure Device Information Service:
    // this->_dis.setManufacturer("BCI Bros");
    // this->_dis.setModel("V1.4");
    // this->_dis.begin();

    // this->_service.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
    // this->_characteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);

    // Enable notifications:
    // this->_characteristic.setProperties(CHR_PROPS_NOTIFY);

    // Configure BLE Service:
    this->_service.begin();
    this->_characteristic.begin();

    Serial.println("Starting advertisement...");

    // Set up advertising packet:
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    // Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_GENERIC_KEYRING);
    Bluefruit.Advertising.addService(this->_service);
    Bluefruit.Advertising.addName();

    // Set up advertising interval:
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244); // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);   // number of seconds in fast mode
    Bluefruit.Advertising.start(0);             // 0 = Don't stop advertising after n seconds

    Serial.println("BLE Initialized.");
}

void BLECommunicator::SendSample(SampleAccumulator &sampleAccumulator)
{
    Serial.println("Sending sample!");
    std::queue<Sample>& samples = sampleAccumulator.getSamples();
    while (samples.size() > 0) {
        Sample sample = samples.front();
        samples.pop();
        double packet[33] = { 0 };
        packet[0] = sample.timestamp;
        memcpy(&packet[1], sample.data, sizeof(sample.data));
        this->_characteristic.notify(packet, sizeof(packet));        

        // Serial.write((uint8_t*)&sample.timestamp, sizeof(sample.timestamp));
        // Serial.write((uint8_t*)&sample.data, sizeof(sample.data));
    }
    // _characteristic.notify("Hello!");
}

bool BLECommunicator::isConnected() const
{
    return this->_connection != nullptr;
}

void BLECommunicator::connect_callback(uint16_t conn_handle)
{
    // Get the reference to current connection
    BLEConnection* connection = Bluefruit.Connection(conn_handle);

    char central_name[32] = { 0 };
    connection->getPeerName(central_name, sizeof(central_name));

    // request PHY changed to 2MB
    connection->requestPHY(BLE_GAP_PHY_2MBPS);

    // request connection interval of 7.5 ms
    Serial.println("Request to change connection params");
    connection->requestConnectionParameter(6, 0, 10); // in unit of 1.25

    _instance->_connection = connection;


    Serial.print("Connected to ");
    Serial.println(central_name);
}

void BLECommunicator::disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void) conn_handle;
    (void) reason;

    _instance->_connection = nullptr;

    Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
    Serial.println("Advertising!");
}
