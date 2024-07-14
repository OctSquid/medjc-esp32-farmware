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
    int16_t value = _mcp.read(2 * index + 1);
    if (index >= 3) value = 0;
    return value;
#endif
};

int16_t ADC::readSME(uint8_t index)
{
#ifdef PIO_UNIT_TESTING
    int16_t value = 2000 + index;
    return value;
#else
    int16_t value = _mcp.read(2 * index) - _mcp.maxValue() / 2;
    if (index >= 3) value = 0;
    return value;
#endif
};
