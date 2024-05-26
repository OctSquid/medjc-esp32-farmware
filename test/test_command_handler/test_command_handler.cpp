#include <unity.h>
#include <CommandHandler.h>
#include <PacketSerial.h>
#include <Version.h>
#include <Adc.h>

#define BUFFER_SIZE 32

// テスト用ブッファ
uint8_t testBuffer[BUFFER_SIZE];

// テスト用送信関数
void testSend(const uint8_t *buffer, size_t size)
{
    memcpy(testBuffer, buffer, size);
}

// Mock Handler class
class MockHandler : public CommandHandler
{
public:
    MockHandler(PacketSerial *packetSerial, ADC *adc) : CommandHandler(packetSerial, adc) {}

    void sendErr(uint8_t errCode) override
    {
        uint8_t errResponse[] = {SERTX, errCode, EERTX};
        testSend(errResponse, sizeof(errResponse));
    }

    void sendResponse(uint8_t cmd, const uint8_t *data, size_t length) override
    {
        uint8_t response[BUFFER_SIZE] = {STX, cmd};
        memcpy(response + 2, data, length);
        response[length + 2] = ETX;
        testSend(response, length + 3);
    }
};

// Mock ADC class
class MockADC : public ADC
{
public:
    int16_t readBaseVoltage() override
    {
        return 32767; // 5.0 V
    }

    bool isConnected(uint8_t index) override
    {
        return index < 2;
    }

    int16_t readME(uint8_t index) override
    {
        int16_t value = 1000 + index;
        return value; // Example values
    }

    int16_t readSME(uint8_t index) override
    {
        int16_t value = 2000 + index;
        return value; // Example values
    }
};

PacketSerial packetSerial;
MockADC mockADC;
MockHandler commandHandler(&packetSerial, &mockADC);
PollingManager pollingManager(&commandHandler, &mockADC);

void test_handleCommand_GetVersion(void)
{
    Command cmd = {CMD_GET_VERSION};
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {0x02, 0x01, VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_GetBaseVoltage(void)
{
    Command cmd = {CMD_GET_BASE_VOLTAGE};
    const int16_t TEST_BASE_VOLTAGE = 32767;
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {0x02, 0x02, highByte(TEST_BASE_VOLTAGE), lowByte(TEST_BASE_VOLTAGE), 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_GetConnections(void)
{
    Command cmd = {CMD_GET_CONNECTIONS};
    commandHandler.handleCommand(cmd);
    // channel 0: 1, channel 1: 1, channel 2: 0, channel 3: 0
    uint8_t expectedResponse[] = {0x02, 0x20, 0x01, 0x01, 0x00, 0x00, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_GetME(void)
{
    Command cmd = {CMD_GET_ME};
    commandHandler.handleCommand(cmd);
    // channel 0: 1000, channel 1: 1001, channel 2: 0, channel 3: 0
    uint8_t expectedResponse[] = {0x02, 0x30, 0x03, 0xE8, 0x03, 0xE9, 0x00, 0x00, 0x00, 0x00, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_GetSME(void)
{
    Command cmd = {CMD_GET_SME};
    commandHandler.handleCommand(cmd);
    // channel 0: 2000, channel 1: 2001, channel 2: 0, channel 3: 0
    uint8_t expectedResponse[] = {0x02, 0x31, 0x07, 0xD0, 0x07, 0xD1, 0x00, 0x00, 0x00, 0x00, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_startPrm(void)
{
    u_int8_t count = 0;
    u_long timer = millis();
    int16_t duration = 1000;
    int16_t interval = 100;
    Command cmd = {CMD_START_PRM};

    // start polling
    pollingManager.setInterval(interval);
    commandHandler.handleCommand(cmd);

    u_int8_t expectedResponse[] = {
        0x02,
        0x40,
        VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH,
        highByte(32767), lowByte(32767),
        0x01, 0x01, 0x00, 0x00,
        highByte(1000), lowByte(1000), highByte(1001), lowByte(1001), 0x00, 0x00, 0x00, 0x00,
        highByte(2000), lowByte(2000), highByte(2001), lowByte(2001), 0x00, 0x00, 0x00, 0x00,
        0x03};

    // clean up buffer
    memset(testBuffer, 0, sizeof(testBuffer));

    String debugMessage = "";
    char message[256];

    while (millis() - timer <= duration)
    {
        pollingManager.update();
        if (testBuffer[0] != 0x00)
        {
            debugMessage = "";
            for (int i = 0; i < sizeof(expectedResponse); i++)
            {
                debugMessage += expectedResponse[i];
                debugMessage += ',';
            }
            debugMessage += '\n';
            memcpy(message, debugMessage.c_str(), debugMessage.length() + 1);
            TEST_ASSERT_MESSAGE(true, message);

            TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
            memset(testBuffer, 0, sizeof(testBuffer));
            count++;
        }
        delay(1);
    }

    pollingManager.stop();

    TEST_ASSERT_GREATER_OR_EQUAL(duration / pollingManager.getInterval(), count);
}

void test_handleCommand_stopPrm(void)
{
    Command cmd = {CMD_STOP_PRM};
    pollingManager.setInterval(100);

    pollingManager.start();
    pollingManager.update();

    commandHandler.handleCommand(cmd);
    pollingManager.update();
    TEST_ASSERT_TRUE(!pollingManager.isRunning());
}

void test_handleCommand_setPRRate(void)
{
    Command cmd = { CMD_SET_PRR, {0x00, 0x64}, 0x02};
    commandHandler.handleCommand(cmd);
    TEST_ASSERT_EQUAL_UINT16(100, pollingManager.getInterval());
}

void test_handleCommand_getPRRate(void)
{
    Command cmd = {CMD_GET_PRR};
    pollingManager.setInterval(100);
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {0x02, 0x43, 0x00, 0x64, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void setup()
{
    UNITY_BEGIN();

    commandHandler.setPollingManager(&pollingManager);

    RUN_TEST(test_handleCommand_GetVersion);
    RUN_TEST(test_handleCommand_GetBaseVoltage);
    RUN_TEST(test_handleCommand_GetConnections);
    RUN_TEST(test_handleCommand_GetME);
    RUN_TEST(test_handleCommand_GetSME);
    RUN_TEST(test_handleCommand_startPrm);
    RUN_TEST(test_handleCommand_stopPrm);
    RUN_TEST(test_handleCommand_setPRRate);
    RUN_TEST(test_handleCommand_getPRRate);

    UNITY_END();
}

void loop()
{
    // No implementation needed for testing
}
