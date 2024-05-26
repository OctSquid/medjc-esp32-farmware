#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdint.h>
#include <stddef.h>

const uint8_t STX = 0x02;
const uint8_t ETX = 0x03;
const uint8_t ACK = 0x06;
const uint8_t SERTX = 0xFE;
const uint8_t EERTX = 0xFD;

enum CommandType
{
    CMD_GET_VERSION = 0x01,
    CMD_GET_BASE_VOLTAGE = 0x02,
    CMD_GET_CONNECTIONS = 0x20,
    CMD_GET_ME = 0x30,
    CMD_GET_SME = 0x31,
    CMD_UNKNOWN
};

struct Command
{
    CommandType type;
    uint8_t params[10]; // パラメータを含む場合のためのバッファ
    size_t paramLength; // パラメータの長さ
};

Command parseCommand(const uint8_t *buffer, size_t size);

#endif // COMMAND_PARSER_H
