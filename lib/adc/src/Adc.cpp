#include <Adc.h>

MCP3208 ADC::_mcp = MCP3208();

void ADC::begin()
{
#ifdef PIO_UNIT_TESTING
    return;
#else
    for (int i = 0; i < 4; ++i)
    {
        SPI.begin(SCK, MISO, MOSI);
        _mcp.setSPIspeed(4000000UL);
        _mcp.begin(CS);
    }
#endif
};

bool ADC::isConnected(uint8_t index)
{
#ifdef PIO_UNIT_TESTING
    return index < 2;
#else
    return index < 3;
#endif
};

void ADC::readAll(int16_t readings[])
{
    int16_t values[8];
    uint8_t channels[8] = {0, 1, 2, 3, 4, 5, 6, 7};
    _mcp.readMultiple(channels, 8, values);
    for (int i = 0; i < 8; ++i)
    {
        readings[i] = values[i];
    }
};

int16_t ADC::readBaseVoltage()
{
#ifdef PIO_UNIT_TESTING
    return 32767;
#else
    int16_t value = _mcp.read(6);
    return value;
#endif
};

int16_t ADC::readME(uint8_t index)
{
#ifdef PIO_UNIT_TESTING
    int16_t value = 1000 + index;
    return value;
#else
    int16_t value = _mcp.read(2 * index + 1) - _mcp.maxValue() / 2;
    if (index >= 3)
    {
        value = 0;
    }
    return value;
#endif
};

void ADC::readMEAll(int16_t readings[])
{
#ifdef PIO_UNIT_TESTING
    for (int i = 0; i < 4; ++i)
    {
        readings[i] = 1000 + i;
    }
#else
    int16_t values[3];
    uint8_t channels[3] = {0, 2, 4};
    _mcp.readMultiple(channels, 3, values);
    for (int i = 0; i < 3; ++i)
    {
        readings[2 * i] = values[i] - _mcp.maxValue() / 2;
    }
    readings[3] = 0;
#endif
};

int16_t ADC::readSME(uint8_t index)
{
#ifdef PIO_UNIT_TESTING
    int16_t value = 2000 + index;
    return value;
#else
    int16_t value = _mcp.read(2 * index);
    if (index >= 3)
    {
        value = 0;
    }
    return value;
#endif
};

void ADC::readSMEAll(int16_t readings[])
{
#ifdef PIO_UNIT_TESTING
    for (int i = 0; i < 4; ++i)
    {
        readings[i] = 2000 + i;
    }
#else
    int16_t values[3];
    uint8_t channels[3] = {1, 3, 5};
    _mcp.readMultiple(channels, 3, values);
    for (int i = 0; i < 3; ++i)
    {
        readings[2 * i] = values[i];
    }
    readings[3] = 0;
#endif
};
