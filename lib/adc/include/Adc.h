#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>
#include <Adafruit_ADS1X15.h>

const uint8_t addr[4] = { 0x48, 0x49, 0x4A, 0x4B };

class ADC
{
private:
    static bool _connections[4];
    static Adafruit_ADS1115 _ads[4];
public:
    ADC() = delete;
    static void begin();
    static bool isConnected(uint8_t index);
    static int16_t readBaseVoltage();
    static int16_t readME(uint8_t index);
    static int16_t readSME(uint8_t index);
};
#endif // ADC_H
