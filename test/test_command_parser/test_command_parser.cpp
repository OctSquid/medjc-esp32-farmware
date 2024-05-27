#include <unity.h>
#include <CommandHandler.h>
#include <CommandParser.h>

// Setup function to initialize Unity test framework
void setUp(void) {
    // Set up any necessary components before each test
}

// Teardown function to clean up after each test
void tearDown(void) {
    // Clean up after each test
}

// Test case to check valid GET_VERSION command
void test_parseCommand_getVersion(void) {
    uint8_t buffer[] = {STX, CMD_GET_VERSION , ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_GET_VERSION, cmd.type);
}

// Test case to check valid GET_BASE_VOLTAGE command
void test_parseCommand_getBaseVoltage(void) {
    uint8_t buffer[] = {STX, CMD_GET_BASE_VOLTAGE, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_GET_BASE_VOLTAGE, cmd.type);
}

// Test case to check unknown command
void test_parseCommand_unknown(void) {
    uint8_t buffer[] = {STX, 0x99, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_UNKNOWN, cmd.type);
}

// Test case to check command with parameters
void test_parseCommand_withParams(void) {
    uint8_t buffer[] = {STX, CMD_GET_ME, 0x10, 0x20, ETX};
    Command cmd = parseCommand(buffer, 5);
    TEST_ASSERT_EQUAL_UINT8(CMD_GET_ME, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(2, cmd.paramLength);
    TEST_ASSERT_EQUAL_UINT8(0x10, cmd.params[0]);
    TEST_ASSERT_EQUAL_UINT8(0x20, cmd.params[1]);
}

// Test case to check invalid frame (missing STX/ETX)
void test_parseCommand_invalidFrame(void) {
    uint8_t buffer[] = {CMD_GET_VERSION, 0x01};
    Command cmd = parseCommand(buffer, sizeof(buffer));
    TEST_ASSERT_EQUAL_UINT8(CMD_UNKNOWN, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(0, cmd.paramLength);
}

void test_parseCommand_startPrm(void) {
    uint8_t buffer[] = {STX, CMD_START_PRM, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_START_PRM, cmd.type);
}

void test_parseCommand_stopPrm(void) {
    uint8_t buffer[] = {STX, CMD_STOP_PRM, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_STOP_PRM, cmd.type);
}

void test_parseCommand_setPrm(void) {
    int16_t value = 200; // 200 ms
    uint8_t buffer[] = {STX, CMD_SET_PRR, highByte(value), lowByte(value), ETX};
    Command cmd = parseCommand(buffer, 5);
    TEST_ASSERT_EQUAL_UINT8(CMD_SET_PRR, cmd.type);
    TEST_ASSERT_EQUAL_UINT8(2, cmd.paramLength);
    TEST_ASSERT_EQUAL_UINT8(highByte(value), cmd.params[0]);
    TEST_ASSERT_EQUAL_UINT8(lowByte(value), cmd.params[1]);
}

void test_parseCommand_getPrm(void) {
    uint8_t buffer[] = {STX, CMD_GET_PRR, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_GET_PRR, cmd.type);
}

void test_parseCommand_getPr(void) {
    uint8_t buffer[] = {STX, CMD_GET_PR, ETX};
    Command cmd = parseCommand(buffer, 3);
    TEST_ASSERT_EQUAL_UINT8(CMD_GET_PR, cmd.type);
}

// Main test runner
void setup() {
    UNITY_BEGIN();

    RUN_TEST(test_parseCommand_getVersion);
    RUN_TEST(test_parseCommand_getBaseVoltage);
    RUN_TEST(test_parseCommand_unknown);
    RUN_TEST(test_parseCommand_withParams);
    RUN_TEST(test_parseCommand_invalidFrame);
    RUN_TEST(test_parseCommand_startPrm);
    RUN_TEST(test_parseCommand_stopPrm);
    RUN_TEST(test_parseCommand_setPrm);
    RUN_TEST(test_parseCommand_getPrm);
    RUN_TEST(test_parseCommand_getPr);

    UNITY_END();
}

void loop() {
    // No implementation needed for testing
}
