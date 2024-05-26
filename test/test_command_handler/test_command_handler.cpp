#include <unity.h>
#include <CommandHandler.h>
#include <PacketSerial.h>
#include <Version.h>
#include <Adc.h>

// テスト用ブッファ
uint8_t testBuffer[16];

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

    void sendAck() override
    {
        uint8_t response[1] = {ACK};
        testSend(response, sizeof(response));
    }

    void sendErr(uint8_t errCode) override
    {
        uint8_t errResponse[3] = {SERTX, errCode, EERTX};
        testSend(errResponse, sizeof(errResponse));
    }

    void sendResponse(uint8_t cmd, const uint8_t *data, size_t length) override
    {
        uint8_t response[16] = {STX, cmd};
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

void setup()
{
    UNITY_BEGIN();

    RUN_TEST(test_handleCommand_GetVersion);
    RUN_TEST(test_handleCommand_GetBaseVoltage);
    RUN_TEST(test_handleCommand_GetConnections);
    RUN_TEST(test_handleCommand_GetME);
    RUN_TEST(test_handleCommand_GetSME);

    UNITY_END();
}

void loop()
{
    // No implementation needed for testing
}
