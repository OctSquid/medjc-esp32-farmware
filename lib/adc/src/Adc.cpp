#include <Adc.h>

ADC::ADC()
{
    for (int i = 0; i < 4; ++i)
    {
        ads[i] = Adafruit_ADS1115();
        ads[i].setDataRate(RATE_ADS1115_860SPS);
        connections[i] = 0;
    }
};

void ADC::begin()
{
    for (int i = 0; i < 4; ++i)
    {
        connections[i] = ads[i].begin(addr[i]);
    }
};

bool ADC::isConnected(uint8_t index)
{
    return connections[index];
};

int16_t ADC::readBaseVoltage()
{
    int16_t value = ads[0].readADC_SingleEnded(3);
    return value;
};

int16_t ADC::readME(uint8_t index)
{
    int16_t value = ads[index].readADC_Differential_0_1();
    return value;
};

int16_t ADC::readSME(uint8_t index)
{
    int16_t value = ads[index].readADC_SingleEnded(3);
    return value;
};
