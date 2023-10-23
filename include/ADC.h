#ifndef ADC_H_
#define ADC_H_

#include <Arduino.h>
#include <ad7779.h>
#include <SPI.h>
#include "SampleData.h"

#define A0 14
#define A1 15
#define A2 16
#define A3 17

struct ADCInit
{
    int32_t startPin;
    int32_t drdyPin;
    int32_t resetPin;
    int32_t chipSelectPin;
};

struct ReadbackHeader
{
    ReadbackHeader(uint8_t header)
    {
        ALERT = header & 0x80;
        CH_ID = (header >> 4) & 0x7;
        RESET_DETECTED = header & 0x8;
        MODULATOR_SATURATE = header & 0x4;
        FILTER_SATURATE = header & 0x2;
        AIN_OV_UVERROR = header & 0x1;
    }

    void print()
    {
        // Serial.print("Channel ID: ");
        Serial.print(CH_ID);
        // Serial.print(", Error: ");
        // Serial.print(ALERT ? "YES" : "NO");
        // Serial.print(", Mod. Saturated: ");
        // Serial.print(MODULATOR_SATURATE ? "YES" : "NO");
        // Serial.print(", Filter Saturated: ");
        // Serial.print(FILTER_SATURATE ? "YES" : "NO");
        // Serial.print(", AIN_OV_UVERROR: ");
        // Serial.print(AIN_OV_UVERROR ? "YES" : "NO");
    }

    bool ALERT;
    uint8_t CH_ID;
    bool RESET_DETECTED;
    bool MODULATOR_SATURATE;
    bool FILTER_SATURATE;
    bool AIN_OV_UVERROR;
};

class ErrorInfo {
public:
    ErrorInfo& appendGen1(uint8_t errors) {
        MEMMAP_CRC_ERR = errors & (1 << 5);
        ROM_CRC_ERR = errors & (1 << 4);
        SPI_CLK_COUNT_ERR = errors & (1 << 3);
        SPI_INVALID_READ_ERR = errors & (1 << 2);
        SPI_INVALID_WRITE_ERR = errors & (1 << 1);
        SPI_CRC_ERR = errors & (1 << 0);

        return *this;
    }

    ErrorInfo& appendGen2(uint8_t errors) {
        RESET_DETECTED = errors & (1 << 5);
        EXT_MCLK_SWITCH_ERR = errors & (1 << 4);
        // Skip bit 3
        ALDO1_PSM_ERR = errors & (1 << 2);
        ALDO2_PSM_ERR = errors & (1 << 1);
        DLDO_PSM_ERR = errors & (1 << 0);

        return *this;
    }

    ErrorInfo& appendStatus1(uint8_t errors) {
        CHIP_ERROR = errors & (1 << 5);
        ERR_LOC_CH4 = errors & (1 << 4);
        ERR_LOC_CH3 = errors & (1 << 3);
        ERR_LOC_CH2 = errors & (1 << 2);
        ERR_LOC_CH1 = errors & (1 << 1);
        ERR_LOC_CH0 = errors & (1 << 0);

        return *this;
    }

    ErrorInfo& appendStatus2(uint8_t errors) {
        CHIP_ERROR = errors & (1 << 5);
        ERR_LOC_GEN2 = errors & (1 << 4);
        ERR_LOC_GEN1 = errors & (1 << 3);
        ERR_LOC_CH7 = errors & (1 << 2);
        ERR_LOC_CH6 = errors & (1 << 1);
        ERR_LOC_CH5 = errors & (1 << 0);

        return *this;
    }

    ErrorInfo& appendStatus3(uint8_t errors) {
        CHIP_ERROR = errors & (1 << 5);
        INIT_COMPLETE = errors & (1 << 4);
        ERR_LOC_SAT_CH6_7 = errors & (1 << 3);
        ERR_LOC_SAT_CH4_5 = errors & (1 << 2);
        ERR_LOC_SAT_CH2_3 = errors & (1 << 1);
        ERR_LOC_SAT_CH0_1 = errors & (1 << 0);

        return *this;
    }

    // General Errors:
    // Reg 1:
    bool MEMMAP_CRC_ERR;
    bool ROM_CRC_ERR;
    bool SPI_CLK_COUNT_ERR;
    bool SPI_INVALID_READ_ERR;
    bool SPI_INVALID_WRITE_ERR;
    bool SPI_CRC_ERR;

    // Reg 2:
    bool RESET_DETECTED;
    bool EXT_MCLK_SWITCH_ERR;
    bool ALDO1_PSM_ERR;
    bool ALDO2_PSM_ERR;
    bool DLDO_PSM_ERR;

    // Status Reg 1:
    bool CHIP_ERROR;
    bool ERR_LOC_CH4;
    bool ERR_LOC_CH3;
    bool ERR_LOC_CH2;
    bool ERR_LOC_CH1;
    bool ERR_LOC_CH0;
    
    // Status Reg 2:
    // bool CHIP_ERROR;
    bool ERR_LOC_GEN2;
    bool ERR_LOC_GEN1;
    bool ERR_LOC_CH7;
    bool ERR_LOC_CH6;
    bool ERR_LOC_CH5;

    // Status Reg 3:
    // bool CHIP_ERROR;
    bool INIT_COMPLETE;
    bool ERR_LOC_SAT_CH6_7;
    bool ERR_LOC_SAT_CH4_5;
    bool ERR_LOC_SAT_CH2_3;
    bool ERR_LOC_SAT_CH0_1;
};

class ADC
{
public:
    ADC();

    int32_t setup(const ADCInit &_adcInit);
    void start(std::function<void(void)> _dataReadyCallback = nullptr);

    void checkStatusRegisters() const;
    void registerDump() const;

    ADCSample readSample();

    void enableSPIControlMode();
    void setupDataISR(voidFuncPtr _dataReadyCallback);

    ad7779_dev *_getRaw() const;
    inline uint8_t getStartPin() const { return _startPin; };
    inline uint8_t getResetPin() const { return _resetPin; };
    inline uint8_t getDrdyPin() const { return _drdyPin; };

private:
    const uint8_t NUM_CHANNELS = 8;

    uint8_t _id;
    ad7779_dev *_adc;
    uint8_t _startPin, _resetPin, _drdyPin;
};

#endif