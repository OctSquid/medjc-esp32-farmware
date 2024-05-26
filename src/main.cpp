#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adc.h>
#include <CommandParser.h>
#include <CommandHandler.h>
#include <PacketSerial.h>

PacketSerial packetSerial;
ADC adc;
CommandHandler commandHandler(&packetSerial, &adc);

void onPacketReceived(const uint8_t* buffer, size_t size) {
  Command cmd = parseCommand(buffer, size);
  commandHandler.handleCommand(cmd);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  adc.begin();

  packetSerial.setStream(&Serial);
  packetSerial.setPacketHandler(&onPacketReceived);
}

void loop() {
  packetSerial.update();
}
