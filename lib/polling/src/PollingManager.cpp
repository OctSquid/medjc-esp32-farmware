#include <PollingManager.h>
#include <Version.h>

CommandHandler *PollingManager::_commandHandler = NULL;
ADC *PollingManager::_adc = NULL;
int16_t PollingManager::_rate = 1000;
Ticker2 PollingManager::_ticker = Ticker2();
volatile bool PollingManager::_isRunning = false;

void PollingManager::init(CommandHandler *commandHandler, ADC *adc)
{
    _commandHandler = commandHandler;
    _adc = adc;
}

bool PollingManager::isRunning()
{
    return _isRunning;
}

void PollingManager::setRate(int16_t rate)
{
    _rate = rate;
    if (_isRunning)
    {
        stop();
        start();
    }
}

int16_t PollingManager::getRate()
{
    return _rate;
}

void PollingManager::start()
{
    if (!_isRunning)
    {
        // Configure hardware timer to call timerISR every second
        _ticker.attach_us((int)(1000000.0 / _rate), &PollingManager::sendReport);
        _isRunning = true;
    }
}

void PollingManager::stop()
{
    if (_isRunning)
    {
        _ticker.detach();
        _isRunning = false;
    }
}

void PollingManager::sendReport()
{
    // ? Note: It takes about 24 ms to get data from the adc.
    uint8_t data[22];
    uint32_t time = micros();

    // base voltage
    int16_t baseVoltage = _adc->readBaseVoltage();
    data[0] = highByte(baseVoltage);
    data[1] = lowByte(baseVoltage);

    for (int i = 0; i < 4; ++i)
    {
        int16_t meValue = 0;
        int16_t smeValue = 0;

        if (_adc->isConnected(i))
        {
            meValue = _adc->readME(i);
            smeValue = _adc->readSME(i);
        }

        // ME
        data[2 * i + 2] = highByte(meValue);
        data[2 * i + 3] = lowByte(meValue);

        // SME
        data[2 * i + 10] = highByte(smeValue);
        data[2 * i + 11] = lowByte(smeValue);
    }
    // time
    data[18] = (time >> 24) & 0xFF;
    data[19] = (time >> 16) & 0xFF;
    data[20] = (time >> 8) & 0xFF;
    data[21] = time & 0xFF;
    _commandHandler->sendResponse(CMD_GET_PR, data, 22);
}
