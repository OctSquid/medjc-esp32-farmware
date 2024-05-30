#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stddef.h>

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;
const uint8_t SERTX = 0xFE;
const uint8_t EERTX = 0xFD;

enum CommandType
{
    CMD_GET_VERSION = 0x01,
    CMD_GET_BASE_VOLTAGE = 0x02,
    CMD_GET_CONNECTIONS = 0x20,
    CMD_GET_ME = 0x30,
    CMD_GET_SME = 0x31,
    CMD_START_PRM = 0x40,
    CMD_STOP_PRM = 0x41,
    CMD_SET_PRR = 0x42,
    CMD_GET_PRR = 0x43,
    CMD_GET_PR = 0x4F,
    CMD_UNKNOWN
};

struct Command
{
    CommandType type;   /**< Command type. */
    uint8_t params[10]; /**< Command parameters. */
    size_t paramLength; /**< Length of the command parameters. */
};

Command parseCommand(const uint8_t *buffer, size_t size);

#endif // COMMAND_PARSER_H
