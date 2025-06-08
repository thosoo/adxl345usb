#include <gtest/gtest.h>
#include "Arduino.h"
#include "mbed.h"

// firmware code compiled with DUAL_SPI via build flags
#include "../../src/main.cpp"

class HandleCommandTest : public ::testing::Test {
protected:
    void SetUp() override {
        Serial.out.clear();
        cmd = "";
        sample_us = 4000; // default
    }
};

TEST(DualSpi, SetupPrintsHeader) {
    Serial.out.clear();
    fakeMicros = 0;
    setup();
    EXPECT_EQ(Serial.out, std::string("time,x0,y0,z0,x1,y1,z1\n"));
}

TEST_F(HandleCommandTest, SetFrequencyValid) {
    cmd = "F=500";
    fakeMicros = 100; // arbitrary
    handleCommand();
    EXPECT_EQ(sample_us, 2000u);
    EXPECT_EQ(Serial.out, std::string("time,x0,y0,z0,x1,y1,z1\n"));
}

TEST_F(HandleCommandTest, SetFrequencyInvalid) {
    cmd = "F=0";
    fakeMicros = 50;
    handleCommand();
    EXPECT_EQ(sample_us, 4000u);
    EXPECT_EQ(Serial.out, std::string(""));
}

TEST_F(HandleCommandTest, HelpCommand) {
    cmd = "H";
    handleCommand();
    EXPECT_EQ(Serial.out, std::string("Commands: F=<1-3200> Hz  | H help\n"));
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
