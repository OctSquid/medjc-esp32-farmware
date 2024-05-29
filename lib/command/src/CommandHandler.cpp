#include <CommandHandler.h>

PacketSerial *CommandHandler::_packetSerial = nullptr;
uint8_t CommandHandler::testBuffer[32];

void CommandHandler::init(PacketSerial *packetSerial)
{
#ifdef PIO_UNIT_TESTING
    memset(testBuffer, 0, sizeof(testBuffer));
#endif
    _packetSerial = packetSerial;
}

void CommandHandler::handleCommand(const Command &cmd)
{
    switch (cmd.type)
    {
    case CMD_GET_VERSION:
        handleGetVersion();
        break;
    case CMD_GET_BASE_VOLTAGE:
        handleGetBaseVoltage();
        break;
    case CMD_GET_CONNECTIONS:
        handleGetConnections();
        break;
    case CMD_GET_ME:
        handleGetME();
        break;
    case CMD_GET_SME:
        handleGetSME();
        break;
    case CMD_START_PRM:
        handleStartPRM();
        break;
    case CMD_STOP_PRM:
        handleStopPRM();
        break;
    case CMD_SET_PRR:
        handleSetPRRate(cmd.params, cmd.paramLength);
        break;
    case CMD_GET_PRR:
        handleGetPRRate();
        break;
    case CMD_GET_PR:
        handleGetPR();
        break;
    default:
        sendErr(0x11);
        break;
    }
}

void CommandHandler::handleGetVersion()
{
    uint8_t versionData[3] = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};
    sendResponse(CMD_GET_VERSION, versionData, 3);
}

void CommandHandler::handleGetBaseVoltage()
{
    int16_t value = ADC::readBaseVoltage();
    uint8_t data[2] = {highByte(value), lowByte(value)};
    sendResponse(CMD_GET_BASE_VOLTAGE, data, 2);
}

void CommandHandler::handleGetConnections()
{
    uint8_t data[4];
    for (int i = 0; i < 4; ++i)
    {
        data[i] = ADC::isConnected(i) ? 0x01 : 0x00;
    }
    sendResponse(CMD_GET_CONNECTIONS, data, 4);
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
    sendResponse(CMD_GET_ME, data, 8);
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
    sendResponse(CMD_GET_SME, data, 8);
};

void CommandHandler::handleStartPRM()
{
    PollingManager::start();
    sendResponse(CMD_START_PRM, {}, 0);
};

void CommandHandler::handleStopPRM()
{
    PollingManager::stop();
    sendResponse(CMD_STOP_PRM, {}, 0);
};

void CommandHandler::handleSetPRRate(const uint8_t *params, size_t length)
{
    int16_t rate = (params[0] << 8) | params[1];
    PollingManager::setRate(rate);
    sendResponse(CMD_SET_PRR, {}, 0);
};

void CommandHandler::handleGetPRRate()
{
    int16_t rate = PollingManager::getRate();
    uint8_t data[2] = {highByte(rate), lowByte(rate)};
    sendResponse(CMD_GET_PRR, data, 2);
};

void CommandHandler::handleGetPR()
{
    PollingManager::sendReport();
};
