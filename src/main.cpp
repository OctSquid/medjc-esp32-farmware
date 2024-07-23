#include <Wire.h>

#include <PacketSerial.h>

#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <PollingManager.h>

PacketSerial packetSerial;

TaskHandle_t packetSerialTaskHandle = NULL;

/**
 * @brief Task to handle the PacketSerial communication.
 */
void packetSerialTask(void *parameter)
{
  while (true)
  {
    packetSerial.update();
    vTaskDelay(1); // Small delay to allow other tasks to run
  }
}

/**
 * @brief Callback function for when a packet is received.
 * @param buffer The buffer containing the packet.
 * @param size The size of the packet.
 */
void onPacketReceived(const uint8_t *buffer, size_t size)
{
  Command cmd = parseCommand(buffer, size);
  CommandHandler::handleCommand(cmd);
}

/**
 * @brief Setup function.
 */
void setup()
{
  Serial.begin(921600);

  Wire.begin();
  Wire.setClock(800000); // Increase I2C clock speed to 800kHz

  ADC::begin();

  CommandHandler::init(&packetSerial);

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);

  // Create the packetSerial task
  xTaskCreateUniversal(
      packetSerialTask,
      "PacketSerialTask",
      2048,
      NULL,
      1,
      &packetSerialTaskHandle,
      1);
}

void loop()
{
  while (1)
  {
    PollingManager::update();
  }
}
