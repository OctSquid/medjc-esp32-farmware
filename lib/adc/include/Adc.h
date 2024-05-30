#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stddef.h>
#include <Adafruit_ADS1X15.h>

const uint8_t addr[4] = {0x48, 0x49, 0x4A, 0x4B};

/**
 * @brief ADC class for interfacing with ADS1115 ADC modules.
 */
class ADC
{
private:
    static bool _connections[4];     /**< Array to store the connection status of each ADC module. */
    static Adafruit_ADS1115 _ads[4]; /**< Array to store the ADS1115 objects for each ADC module. */
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
