#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include <gtest/gtest.h>
#include <fstream>
#include <climits>
#include "Logger.h"

namespace
{
    class A
    {
        int first = 34;
        double precision = 0.003;

        friend std::string typeToString(const A& obj);
    };

    std::string typeToString(const A& obj)
    {
        return "type A { first = " + std::to_string(obj.first)
            + " precision = " + std::to_string(obj.precision) + "}";
    }
}

class LoggerTestFixture : public ::testing::Test
{
protected:
    std::string logFilePath = "logFile.txt";
    const int defaultFlags = eLogSettings::UseFile | eLogSettings::OpenCloseFile;

    void SetUp() override
    {
        Logger::adjustSettings(defaultFlags);
        Logger::setLogFilePath(logFilePath, false);
    }

    void TearDown() override
    {
        // erase file content
        std::ofstream file(logFilePath, std::ofstream::out | std::ios::trunc);
        file.close();
    }

    bool GetLogFileText(std::string& fileText) const
    {
        std::ifstream inputFile(logFilePath);
        if (inputFile.is_open())
        {
            std::string line;
            while (std::getline(inputFile, line))
            {
                fileText += line;
            }
            inputFile.close();
            return true;
        }
        return false;
    }
};

TEST_F(LoggerTestFixture, LogFileCreation)
{
    Logger::print("Some random log text");
    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_EQ(text, "Some random log text");
}

TEST_F(LoggerTestFixture, TurnOnOffTest)
{
    Logger::print("FirstLine");
    Logger::turnOff();
    Logger::print("SecondLine");
    Logger::turnOn();
    Logger::print("ThirdLine");

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));

    EXPECT_NE(text.find("FirstLine"), std::string::npos);
    EXPECT_EQ(text.find("SecondLine"), std::string::npos);
    EXPECT_NE(text.find("ThirdLine"), std::string::npos);
}

TEST_F(LoggerTestFixture, ShowThreadIDTest)
{
    Logger::adjustSettings(defaultFlags | eLogSettings::ShowThreadID);
    Logger::print("Test log message");

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_NE(text.find("[thread"), std::string::npos);
}

TEST_F(LoggerTestFixture, ShowInfoTag)
{
    Logger::print("Test log message", eLogMsgType::Info);

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_NE(text.find("[Info]"), std::string::npos);
}

TEST_F(LoggerTestFixture, ShowWarningTag)
{
    Logger::print("Test log message", eLogMsgType::Warning);

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_NE(text.find("[Warning]"), std::string::npos);
}

TEST_F(LoggerTestFixture, ShowErrorTag)
{
    Logger::print("Test log message", eLogMsgType::Error);

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_NE(text.find("[ERROR]"), std::string::npos);
}

TEST_F(LoggerTestFixture, ShowTimeStamp)
{
    Logger::startTimer("Timer start");

    //dummy cycle, not sure if it will be present in optimized code :)
    for (size_t i = 0; i < LONG_MAX; ++i);

    Logger::stopTimer(eLogTimerUnits::Milliseconds, "Timer stop");

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_NE(text.find("[timer start]"), std::string::npos);
    EXPECT_NE(text.find("[timer stop"), std::string::npos);
}

TEST_F(LoggerTestFixture, PrintFormattedString)
{
    Logger::print(eLogMsgType::None, "The value of num1 is % d and num2 is % .2f", 10, 3.14);

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_EQ(text, "The value of num1 is  10 and num2 is  3.14");
}

TEST_F(LoggerTestFixture, PrintCustomType)
{
    A objA;
    Logger::printObject(objA);

    std::string text;
    EXPECT_TRUE(GetLogFileText(text));
    EXPECT_EQ(text, "type A { first = 34 precision = 0.003000}");
}

