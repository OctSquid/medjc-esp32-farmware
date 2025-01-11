#include <CommandParser.h>
#include <string.h>

Command parseCommand(const uint8_t *buffer, size_t size)
{
  Command cmd;
  cmd.paramLength = 0;

  if (size < 2)
  {
    cmd.type = CMD_UNKNOWN;
    return cmd;
  }

  uint8_t commandType = buffer[0];

  switch (commandType)
  {
  case 0x00:
    cmd.type = CMD_PING;
    break;
  case 0x01:
    cmd.type = CMD_GET_VERSION;
    break;
  case 0x02:
    cmd.type = CMD_GET_BASE_VOLTAGE;
    break;
  case 0x20:
    cmd.type = CMD_GET_CONNECTIONS;
    break;
  case 0x30:
    cmd.type = CMD_GET_ME;
    break;
  case 0x31:
    cmd.type = CMD_GET_SME;
    break;
  case 0x40:
    cmd.type = CMD_START_PRM;
    break;
  case 0x41:
    cmd.type = CMD_STOP_PRM;
    break;
  case 0x42:
    cmd.type = CMD_SET_PRR;
    break;
  case 0x43:
    cmd.type = CMD_GET_PRR;
    break;
  case 0x4F:
    cmd.type = CMD_GET_PR;
    break;
  default:
    cmd.type = CMD_UNKNOWN;
    break;
  }

  cmd.id = (buffer[1] << 8) | buffer[2];

  if (size > 3)
  {
    cmd.paramLength = size - 3; // cmd.paramLength is the size of the buffer minus the command type and id
    memcpy(cmd.params, buffer + 3, cmd.paramLength); // cmd.params is the buffer starting from the 4th element because the first 3 elements are the command type and id
  }

  return cmd;
}
