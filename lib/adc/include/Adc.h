#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>

#include <SPI.h>

#include "MCP_ADC.h"

#define SCK 4
#define MISO 7
#define MOSI 8
#define CS 9

/**
 * @brief ADC class for interfacing with MCP3208 ADC modules.
 */
class ADC
{
private:
    static MCP3208 _mcp;
public:
    ADC() = delete;

    /**
     * @brief Initializes the ADC modules.
     */
    static void begin();

    /**
     * @brief Checks if an ADC module is connected.
     * @param index The index of the ADC module.
     * @return True if the ADC module is connected, false otherwise.
     */
    static bool isConnected(uint8_t index);

    /**
     * @brief Reads the base voltage from the ADC module.
     * @return The base voltage value.
     */
    static int16_t readBaseVoltage();

    /**
     * @brief Reads the ME (Micro Electrode) value from the specified ADC module.
     * @param index The index of the ADC module.
     * @return The ME value.
     */
    static int16_t readME(uint8_t index);

    /**
     * @brief Reads the SME (Sub-Micro Electrode) value from the specified ADC module.
     * @param index The index of the ADC module.
     * @return The SME value.
     */
    static int16_t readSME(uint8_t index);
};
#endif // ADC_H
