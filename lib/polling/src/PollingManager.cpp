#include <PollingManager.h>
#include <Version.h>

PollingManager::PollingManager(CommandHandler *commandHandler, ADC *adc)
{
    this->commandHandler = commandHandler;
    this->commandHandler->setPollingManager(this);
    this->adc = adc;
}

bool PollingManager::isRunning()
{
    return this->_isRunning;
}

void PollingManager::setRate(int16_t rate)
{
    this->_rate = rate;
}

int16_t PollingManager::getRate()
{
    return this->_rate;
}

void PollingManager::start()
{
    if (!this->_isRunning)
    {
        this->_ticker.attach(1.0 / this->_rate, timerCallback, this);
        this->_isRunning = true;
    }
}

void PollingManager::stop()
{
    this->_ticker.detach();
    this->_isRunning = false;
}

void PollingManager::sendReport()
{
    // ? Note: It takes about 24 ms to get data from the adc.
    uint8_t data[22];
    uint32_t time = millis();

    // base voltage
    int16_t baseVoltage = this->adc->readBaseVoltage();
    data[0] = highByte(baseVoltage);
    data[1] = lowByte(baseVoltage);

    for (int i = 0; i < 4; ++i)
    {
        int16_t meValue = 0;
        int16_t smeValue = 0;

        if (this->adc->isConnected(i))
        {
            meValue = this->adc->readME(i);
            smeValue = this->adc->readSME(i);
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
    this->commandHandler->sendResponse(CMD_GET_PR, data, 22);
}

void PollingManager::timerCallback(PollingManager *instance)
{
    instance->sendReport();
}
