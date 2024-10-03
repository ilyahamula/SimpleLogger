#include <iostream>
#include "Logger.h"
#include "ClassExamples.h"

int main(int argc, char* argv[])
{
    std::string logFile;
    if (argc == 2)
        logFile = argv[1];
    else
        Logger::print("No log file path was provided.", eLogMsgType::Warning);

    constexpr int settingsFlag = eLogSettings::UseCout | eLogSettings::UseFile;
    Logger::adjustSettings(settingsFlag);
    Logger::setLogFilePath("D:\\loggerOutput.txt");
    Logger::print("Example of simple log message");
    Logger::print("Example of error message", eLogMsgType::Error);
    Logger::print("Example of warning message", eLogMsgType::Warning);
    Logger::print("Example of info message", eLogMsgType::Info);
    Logger::print("\n------------------------------------------\n");
    Logger::print("Next output describe work of Logger in multiple threads with time calculation:", eLogMsgType::Info);

    Logger::adjustSettings(settingsFlag | eLogSettings::ShowThreadID);

    constexpr uint8_t threadNum = 5;
    std::thread threads[threadNum];
    int sleepTimes[threadNum] = { 1000, 3500, 2000, 4000, 3500 }; // Milliseconds

    for (int i = 0; i < threadNum; ++i)
    {
        threads[i] = std::thread([](int id, int sleepTime) {
            Logger::startTimer("Thread " + std::to_string(id));
            std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
            Logger::stopTimer(eLogTimerUnits::Milliseconds, "Thread " + std::to_string(id) + " end of execution");
            },
            i + 1, sleepTimes[i]);
    }

    for (int i = 0; i < threadNum; ++i)
        threads[i].join();

    Logger::adjustSettings(settingsFlag);
    Logger::print("\n------------------------------------------\n");
    Logger::print("Next output describe logging of user types:", eLogMsgType::Info);

    A objA;
    Logger::printObject(objA);
    B objB;
    Logger::printObject(objB);
}
