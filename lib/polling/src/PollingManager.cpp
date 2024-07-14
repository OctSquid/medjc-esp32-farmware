#include <PollingManager.h>
#include <Version.h>

int16_t PollingManager::_rate = 1000;
Ticker2 PollingManager::_ticker = Ticker2();
volatile bool PollingManager::_isRunning = false;
volatile bool PollingManager::_flag_isr = false;

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
        _ticker.attach_us(1000000ULL / _rate, &PollingManager::onTimer);
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

void IRAM_ATTR PollingManager::sendReport()
{
    uint8_t data[22];
    uint32_t time = micros();

    // base voltage
    int16_t baseVoltage = ADC::readBaseVoltage();
    data[0] = highByte(baseVoltage);
    data[1] = lowByte(baseVoltage);

    for (int i = 0; i < 4; ++i)
    {
        int16_t meValue = 0;
        int16_t smeValue = 0;

        if (ADC::isConnected(i))
        {
            meValue = ADC::readME(i);
            smeValue = ADC::readSME(i);
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
    CommandHandler::sendResponse(CMD_GET_PR, data, 22);
}

void PollingManager::onTimer()
{
    _flag_isr = true;
}
