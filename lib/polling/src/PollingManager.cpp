#include <PollingManager.h>
#include <Version.h>

PollingManager::PollingManager(CommandHandler *commandHandler, ADC *adc) {
    this->commandHandler = commandHandler;
    this->commandHandler->setPollingManager(this);
    this->adc = adc;
}

bool PollingManager::isRunning() {
    return this->_isRunning;
}

void PollingManager::setInterval(int16_t _interval) {
    this->_interval = _interval;
}

int16_t PollingManager::getInterval() {
    return this->_interval;
}

void PollingManager::start() {
    this->_timer = millis();
    this->_isRunning = true;
}

void PollingManager::stop() {
    this->_timer = 0;
    this->_isRunning = false;
}

void PollingManager::update() {
    if (!this->_isRunning) {
        return;
    }

    if (millis() - this->_timer >= this->_interval) {
        this->sendReport();
        this->_timer = millis();
    }
}

void PollingManager::sendReport() {
    uint8_t data[18];

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

    this->commandHandler->sendResponse(CMD_GET_PR, data, 18);
}
