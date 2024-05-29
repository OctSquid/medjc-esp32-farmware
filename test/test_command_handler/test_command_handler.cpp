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
    MockHandler(PacketSerial *packetSerial) : CommandHandler(packetSerial) {}

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

PacketSerial packetSerial;
MockHandler commandHandler(&packetSerial);

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
    u_int64_t timer = micros();
    u_int64_t duration = 1001 * 1000;
    int16_t rate = 1000;
    Command cmd = {CMD_START_PRM};

    // start polling
    PollingManager::setRate(rate);
    commandHandler.handleCommand(cmd);

    uint8_t expectedResponse[] = {0x02, 0x40, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));

    u_int8_t expectedPollingReport[] = {
        0x02,
        0x4F,
        highByte(32767), lowByte(32767),                                                      // 2
        highByte(1000), lowByte(1000), highByte(1001), lowByte(1001), 0x00, 0x00, 0x00, 0x00, // 8
        highByte(2000), lowByte(2000), highByte(2001), lowByte(2001), 0x00, 0x00, 0x00, 0x00, // 8
        0x00, 0x00, 0x00, 0x00,                                                               // time stamp
        0x03};

    // clean up buffer
    memset(testBuffer, 0, sizeof(testBuffer));

    while (micros() - timer <= duration)
    {
        if (testBuffer[0] != 0x00)
        {
            //TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedPollingReport, testBuffer, sizeof(expectedPollingReport) - 5);
            memset(testBuffer, 0, sizeof(testBuffer));
            count++;
        }
    }

    PollingManager::stop();

    TEST_ASSERT_GREATER_OR_EQUAL((int)(duration / 1000000.0 * PollingManager::getRate()), count);
}

void test_handleCommand_stopPrm(void)
{
    Command cmd = {CMD_STOP_PRM};
    PollingManager::setRate(100);

    PollingManager::start();

    commandHandler.handleCommand(cmd);

    uint8_t expectedResponse[] = {0x02, 0x41, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
    TEST_ASSERT_TRUE(!PollingManager::isRunning());
}

void test_handleCommand_setPRRate(void)
{
    Command cmd = {CMD_SET_PRR, {0x00, 0x64}, 0x02}; // 100 Hz
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {0x02, 0x42, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
    TEST_ASSERT_EQUAL_UINT16(100, PollingManager::getRate());
}

void test_handleCommand_getPRRate(void)
{
    Command cmd = {CMD_GET_PRR};
    PollingManager::setRate(100);
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {0x02, 0x43, 0x00, 0x64, 0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse));
}

void test_handleCommand_getPR(void)
{
    Command cmd = {CMD_GET_PR};
    uint32_t time = millis();
    commandHandler.handleCommand(cmd);
    uint8_t expectedResponse[] = {
        0x02,
        0x4F,
        highByte(32767), lowByte(32767),                                                      // 2
        highByte(1000), lowByte(1000), highByte(1001), lowByte(1001), 0x00, 0x00, 0x00, 0x00, // 8
        highByte(2000), lowByte(2000), highByte(2001), lowByte(2001), 0x00, 0x00, 0x00, 0x00, // 8
        0x00, 0x00, 0x00, 0x00,          // time stamp
        0x03};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expectedResponse, testBuffer, sizeof(expectedResponse) - 5);
    TEST_ASSERT_LESS_OR_EQUAL(1, (u_int32_t)(testBuffer[18] << 24 | testBuffer[19] << 16 | testBuffer[20] << 8 | testBuffer[21]) - time);
}

void setup()
{
    UNITY_BEGIN();

    PollingManager::init(&commandHandler);

    RUN_TEST(test_handleCommand_GetVersion);
    RUN_TEST(test_handleCommand_GetBaseVoltage);
    RUN_TEST(test_handleCommand_GetConnections);
    RUN_TEST(test_handleCommand_GetME);
    RUN_TEST(test_handleCommand_GetSME);
    RUN_TEST(test_handleCommand_startPrm);
    RUN_TEST(test_handleCommand_stopPrm);
    RUN_TEST(test_handleCommand_setPRRate);
    RUN_TEST(test_handleCommand_getPRRate);
    RUN_TEST(test_handleCommand_getPR);

    UNITY_END();
}

void loop()
{
    // No implementation needed for testing
}
