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

class CommandHandler;

class PollingManager
{
private:
    static void timerCallback(PollingManager *instance);

    CommandHandler *commandHandler;
    ADC *adc;
    int16_t _rate = 10;
    bool _isRunning = false;
    Ticker _ticker;

public:
    PollingManager(CommandHandler *commandHandler, ADC *adc);
    void sendReport();
    void setRate(int16_t rate);
    int16_t getRate();
    bool isRunning();
    void start();
    void stop();
};

#endif // POLLING_MANAGER_H
