#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <PacketSerial.h>
#include <PollingManager.h>

PacketSerial packetSerial;
CommandHandler commandHandler(&packetSerial);

TaskHandle_t  packetSerialTaskHandle = NULL;

// Task function for packetSerial.update
void packetSerialTask(void *parameter)
{
  while (true)
  {
    packetSerial.update();
    vTaskDelay(1); // Small delay to allow other tasks to run
  }
}

void onPacketReceived(const uint8_t *buffer, size_t size)
{
  Command cmd = parseCommand(buffer, size);
  commandHandler.handleCommand(cmd);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(800000); // Increase I2C clock speed to 800kHz

  ADC::begin();

  PollingManager::init(&commandHandler);

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);

  // Create the packetSerial task
  xTaskCreate(
      packetSerialTask,
      "PacketSerialTask",
      2048,
      NULL,
      1,
      &packetSerialTaskHandle);
}

void loop()
{
  // Empty loop since tasks are handled by FreeRTOS
}
