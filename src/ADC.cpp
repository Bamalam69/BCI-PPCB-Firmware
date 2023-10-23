#include "ADC.h"
#include <Arduino.h>
#include "SampleData.h"

ADC::ADC()
{
}

int32_t ADC::setup(const ADCInit &_adcInit)
{
    ad7779_init_param init_param;
    init_param.ctrl_mode = ad7779_ctrl_mode::AD7779_SPI_CTRL;

#ifdef DEBUG
    Serial.println("Set this as head ADC.");
#endif

    init_param.gpio_drdy = {
        .number = _adcInit.drdyPin,
    };
    _drdyPin = _adcInit.drdyPin;
    // }
    _startPin = _adcInit.startPin;
    init_param.gpio_start = {
        .number = _adcInit.startPin,
    };
    init_param.gpio_reset = {
        .number = _adcInit.resetPin,
    };
    _resetPin = _adcInit.resetPin;

    init_param.spi_crc_en = ad7779_state::AD7779_DISABLE;
    init_param.spi_init.chip_select = _adcInit.chipSelectPin;
    // init_param.spi_init.spiClass = &_adcInit.spiPort,
    init_param.spi_init.bit_order = no_os_spi_bit_order::NO_OS_SPI_BIT_ORDER_MSB_FIRST;
    init_param.pwr_mode = ad7779_pwr_mode::AD7779_HIGH_RES;
    init_param.dec_rate_int = 0x300;
    // init_param.dec_rate_int = 0x280;
    // init_param.dec_rate_int = 0x200;
    // init_param.dec_rate_int = 0x100;
    // init_param.dec_rate_int = 0x64;
    // init_param.dec_rate_int = 0xfa0;
    init_param.dec_rate_dec = 0;
    init_param.ref_type = ad7779_ref_type::AD7779_INT_REF;
    init_param.read_from_cache = false;

    for (uint8_t i = AD7779_CH0; i <= AD7779_CH7; i++)
    {
        init_param.state[i] = ad7779_state::AD7779_ENABLE;
        init_param.gain[i] = ad7779_gain::AD7779_GAIN_8;
        // init_param.sync_offset[i] = 0;
        // init_param.offset_corr[i] = 0x555555;
        // init_param.offset_corr[i] = offset_corr[i];
        init_param.gain_corr[i] = 0xD55554; // 0x555555; // 0xD55554; ; // 0x555555;
    }

    return ad7779_init(&_adc, init_param);
}

void ADC::start(std::function<void(void)> _dataReadyCallback)
{
#ifdef DEBUG
    Serial.println("Starting!");
#endif

    ad7779_set_spi_op_mode(_adc, ad7779_spi_op_mode::AD7779_SD_CONV);

#ifdef DEBUG
    Serial.println("Flashing START...");

    Serial.print("Start pin: ");
    Serial.println(_startPin);
#endif

    digitalWrite(_startPin, LOW);
    delay(10);
    digitalWrite(_startPin, HIGH);
}

void ADC::checkStatusRegisters() const
{
    ad7779_check_status_regs(_adc);
}

void ADC::registerDump() const
{
    ad7779_register_dump(_adc);
}

/// @brief  Reads a single sample (all 8 channels) from the ADC.
/// @return A struct containing the sample data.
ADCSample ADC::readSample()
{
    ADCSample sampleData;
    for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++)
    {
        const uint8_t bytes_number = 4;
        // Need a preliminary empty message to send,
        //  in order to read a register.
        // Zeroes are sent so as to not 
        //  Initiate an SPI software reset of the AFE.
        uint8_t data[] = {0x80, 0x00, 0x00, 0x00};
        no_os_spi_write_and_read(_adc->spi_desc, data, bytes_number);

        int32_t value = (data[1] << 16) | (data[2] << 8) | data[3];

        // Some two's complement sign hack: 
        // (Since the sign bit is at the 24th bit...)
        value <<= 8; value >>= 8;

        // Pack into data struct:
        sampleData.data[channel] = value;
    }
    return sampleData;
}

ad7779_dev *ADC::_getRaw() const
{
    return _adc;
}

void ADC::enableSPIControlMode()
{
    ad7779_set_spi_op_mode(_adc, ad7779_spi_op_mode::AD7779_SD_CONV);
}

void ADC::setupDataISR(voidFuncPtr _dataReadyCallback)
{
    attachInterrupt(digitalPinToInterrupt(getDrdyPin()), _dataReadyCallback, FALLING);
}
