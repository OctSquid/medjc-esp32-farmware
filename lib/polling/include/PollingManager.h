#ifndef POLLING_MANAGER_H
#define POLLING_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#include <PacketSerial.h>
#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <Version.h>

class CommandHandler;

// Function declarations
class PollingManager
{
private:
    CommandHandler *commandHandler;
    ADC *adc;
    int16_t _interval = 100;
    long _timer = 0;
    bool _isRunning = false;
public:
    PollingManager(CommandHandler *commandHandler, ADC *adc);
    void sendReport();
    void setInterval(int16_t _interval);
    int16_t getInterval();
    bool isRunning();
    void start();
    void stop();
    void update();
};

#endif // POLLING_MANAGER_H
