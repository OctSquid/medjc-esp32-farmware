#ifndef POLLING_MANAGER_H
#define POLLING_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <functional>

#include <PacketSerial.h>
#include <Ticker2.h>

#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <Version.h>

class CommandHandler;

/**
 * @brief Polling manager class.
 */
class PollingManager final
{
private:
    static int16_t _rate;            /**< Polling rate in Hz. */
    static volatile bool _isRunning; /**< Flag to indicate if the polling manager is running. */
    static Ticker2 _ticker;          /**< Ticker object for the polling manager. */
    static volatile bool _flag_isr;  /**< Flag to indicate if the ISR has been triggered. */

public:
    PollingManager() = delete;

    static void IRAM_ATTR sendReport();

    /**
     * @brief Updates the polling manager.
     * ! This function must be called in the main loop.
     */
    static void IRAM_ATTR inline update()
    {
        if (_flag_isr)
        {
            _flag_isr = false;
            sendReport();
        }
    };
    static void IRAM_ATTR onTimer();
    static void setRate(int16_t rate);
    static int16_t getRate();
    static bool isRunning();
    static void start();
    static void stop();
};

#endif // POLLING_MANAGER_H
