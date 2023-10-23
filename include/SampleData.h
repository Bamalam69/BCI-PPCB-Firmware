#ifndef SAMPLE_DATA_H_
#define SAMPLE_DATA_H_

// Definition for holding a single sample from a single ADC:
class ADCSample
{
private:
    static constexpr uint8_t NUM_CHANNELS_PER_ADC = 8;

public:
    int32_t data[ADCSample::NUM_CHANNELS_PER_ADC];
};

class Sample
{
private:
    static constexpr uint8_t NUM_CHANNELS_PER_ADC = 8;
    static constexpr uint8_t NUM_ADCS = 4;
public:
    uint32_t timestamp;
    int32_t data[Sample::NUM_CHANNELS_PER_ADC * Sample::NUM_ADCS];
};

#endif // SAMPLE_DATA_H_