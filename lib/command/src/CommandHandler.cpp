#include <CommandHandler.h>

CommandHandler::CommandHandler(PacketSerial *packetSerial, ADC *adc)
{
    this->packetSerial = packetSerial;
    this->adc = adc;
}

void CommandHandler::handleCommand(const Command &cmd)
{
    this->sendAck();
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
    default:
        this->sendErr(0x11);
        break;
    }
}

void CommandHandler::sendAck()
{
    uint8_t response[1] = {ACK};
    this->packetSerial->send(response, sizeof(response));
}

void CommandHandler::sendErr(uint8_t errCode)
{
    uint8_t errResponse[3] = {SERTX, errCode, EERTX};
    this->packetSerial->send(errResponse, sizeof(errResponse));
}

void CommandHandler::sendResponse(uint8_t cmd, const uint8_t *data, size_t length)
{
    uint8_t response[16] = {STX, cmd};
    memcpy(response + 2, data, length);
    response[length + 2] = ETX;
    this->packetSerial->send(response, length + 3);
}

void CommandHandler::handleGetVersion()
{
    uint8_t versionData[3] = {VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH};
    this->sendResponse(CMD_GET_VERSION, versionData, 3);
}

void CommandHandler::handleGetBaseVoltage()
{
    int16_t value = this->adc->readBaseVoltage();
    uint8_t data[2] = {highByte(value), lowByte(value)};
    this->sendResponse(CMD_GET_BASE_VOLTAGE, data, 2);
}

void CommandHandler::handleGetConnections()
{
    uint8_t data[4];
    for (int i = 0; i < 4; ++i)
    {
        data[i] = this->adc->isConnected(i) ? 0x01 : 0x00;
    }
    this->sendResponse(CMD_GET_CONNECTIONS, data, 4);
}

void CommandHandler::handleGetME()
{
    uint8_t data[8];
    for (int i = 0; i < 4; ++i)
    {
        int16_t value;
        if (this->adc->isConnected(i))
        {
            value = this->adc->readME(i);
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
        if (this->adc->isConnected(i))
        {
            value = this->adc->readSME(i);
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
