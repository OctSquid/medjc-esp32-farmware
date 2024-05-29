#include <CommandHandler.h>

CommandHandler::CommandHandler(PacketSerial *packetSerial)
{
    this->_packetSerial = packetSerial;
}

void CommandHandler::handleCommand(const Command &cmd)
{
    switch (cmd.type)
    {
    case CMD_GET_VERSION:
        this->handleGetVersion();
        break;
    case CMD_GET_BASE_VOLTAGE:
        this->handleGetBaseVoltage();
        break;
    case CMD_GET_CONNECTIONS:
        this->handleGetConnections();
        break;
    case CMD_GET_ME:
        this->handleGetME();
        break;
    case CMD_GET_SME:
        this->handleGetSME();
        break;
    case CMD_START_PRM:
        this->handleStartPRM();
        break;
    case CMD_STOP_PRM:
        this->handleStopPRM();
        break;
    case CMD_SET_PRR:
        this->handleSetPRRate(cmd.params, cmd.paramLength);
        break;
    case CMD_GET_PRR:
        this->handleGetPRRate();
        break;
    case CMD_GET_PR:
        this->handleGetPR();
        break;
    default:
        this->sendErr(0x11);
        break;
    }
}

void CommandHandler::sendErr(uint8_t errCode)
{
    uint8_t errResponse[3] = {SERTX, errCode, EERTX};
    this->_packetSerial->send(errResponse, sizeof(errResponse));
}

void CommandHandler::sendResponse(uint8_t cmd, const uint8_t *data, size_t length)
{
    uint8_t response[32] = {STX, cmd};
    if (length != 0) {
        memcpy(response + 2, data, length);
    }
    response[length + 2] = ETX;
    this->_packetSerial->send(response, length + 3);
}

void CommandHandler::handleGetVersion()
{
    uint8_t versionData[3] = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};
    this->sendResponse(CMD_GET_VERSION, versionData, 3);
}

void CommandHandler::handleGetBaseVoltage()
{
    int16_t value = ADC::readBaseVoltage();
    uint8_t data[2] = {highByte(value), lowByte(value)};
    this->sendResponse(CMD_GET_BASE_VOLTAGE, data, 2);
}

void CommandHandler::handleGetConnections()
{
    uint8_t data[4];
    for (int i = 0; i < 4; ++i)
    {
        data[i] = ADC::isConnected(i) ? 0x01 : 0x00;
    }
    this->sendResponse(CMD_GET_CONNECTIONS, data, 4);
}

void CommandHandler::handleGetME()
{
    uint8_t data[8];
    for (int i = 0; i < 4; ++i)
    {
        int16_t value;
        if (ADC::isConnected(i))
        {
            value = ADC::readME(i);
        }
        else
        {
            value = 0;
        }
        data[2 * i] = highByte(value);
        data[2 * i + 1] = lowByte(value);
    }
    this->sendResponse(CMD_GET_ME, data, 8);
}

void CommandHandler::handleGetSME()
{
    uint8_t data[8];
    for (int i = 0; i < 4; ++i)
    {
        int16_t value;
        if (ADC::isConnected(i))
        {
            value = ADC::readSME(i);
        }
        else
        {
            value = 0;
        }
        data[2 * i] = highByte(value);
        data[2 * i + 1] = lowByte(value);
    }
    this->sendResponse(CMD_GET_SME, data, 8);
};

void CommandHandler::handleStartPRM() {
    PollingManager::start();
    this->sendResponse(CMD_START_PRM, {}, 0);
};

void CommandHandler::handleStopPRM() {
    PollingManager::stop();
    this->sendResponse(CMD_STOP_PRM, {}, 0);
};

void CommandHandler::handleSetPRRate(const uint8_t *params, size_t length) {
    int16_t rate = (params[0] << 8) | params[1];
    PollingManager::setRate(rate);
    this->sendResponse(CMD_SET_PRR, {}, 0);
};

void CommandHandler::handleGetPRRate() {
    int16_t rate = PollingManager::getRate();
    uint8_t data[2] = {highByte(rate), lowByte(rate)};
    this->sendResponse(CMD_GET_PRR, data, 2);
};

void CommandHandler::handleGetPR() {
    PollingManager::sendReport();
};
