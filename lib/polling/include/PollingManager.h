#ifndef POLLING_MANAGER_H
#define POLLING_MANAGER_H

#include <Ticker.h>
#include <functional>

#include <stdint.h>
#include <stddef.h>

#include <PacketSerial.h>
#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <Version.h>
#include <Ticker2.h>

class CommandHandler;

class PollingManager final
{
private:
    static int16_t _rate;
    static volatile bool _isRunning;
    static Ticker2 _ticker;
    static volatile bool _flag_isr;

public:
    PollingManager() = delete;
    static void IRAM_ATTR sendReport();
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
