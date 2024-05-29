#include <Adc.h>

Adafruit_ADS1115 ADC::_ads[4] = {Adafruit_ADS1115(), Adafruit_ADS1115(), Adafruit_ADS1115(), Adafruit_ADS1115()};
bool ADC::_connections[4] = {false, false, false, false};

void ADC::begin()
{
    for (int i = 0; i < 4; ++i)
    {
        _ads[i].setDataRate(RATE_ADS1115_860SPS);
        _connections[i] = _ads[i].begin(addr[i]);
    }
};

bool ADC::isConnected(uint8_t index)
{
    return _connections[index];
};

int16_t ADC::readBaseVoltage()
{
    int16_t value = _ads[0].readADC_SingleEnded(3);
    return value;
};

int16_t ADC::readME(uint8_t index)
{
    int16_t value = _ads[index].readADC_Differential_0_1();
    return value;
};

int16_t ADC::readSME(uint8_t index)
{
    int16_t value = _ads[index].readADC_SingleEnded(2);
    return value;
};
