#include "Logger.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

namespace
{
#define FAILED_TIME_MEASUREMENT -1

    enum class ePrintColor : uint8_t
    {
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Yellow = 6,
        White = 7
    };
#ifdef _WIN32
    #include <windows.h>

    void PrintInColor(ePrintColor color, const std::string& line, std::ostream& out, bool isCerr = true)
    {
        HANDLE outputHandle = GetStdHandle(isCerr ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(outputHandle, static_cast<uint8_t>(color));
        out << line;
        SetConsoleTextAttribute(outputHandle, static_cast<uint8_t>(ePrintColor::White));
    }

    std::string GetProcessID()
    {
        return std::to_string(GetCurrentProcessId());
    }
#else
    #include <unistd.h>

    void PrintInColor(ePrintColor color, const std::string& line, std::ostream& out, bool dummy = true)
    {
        if (color == ePrintColor::Blue)
            out << "\033[0;34m" << line << "\033[0m";
        else if (color == ePrintColor::Green)
            out << "\033[0;32m" << line << "\033[0m";
        else if (color == ePrintColor::Cyan)
            out << "\033[0;36m" << line << "\033[0m";
        else if (color == ePrintColor::Red)
            out << "\033[0;31m" << line << "\033[0m";
        else if (color == ePrintColor::Magenta)
            out << "\033[0;35m" << line << "\033[0m";
        else if (color == ePrintColor::Yellow)
            out << "\033[0;33m" << line << "\033[0m";
    }

    std::string GetProcessID()
    {
        return std::to_string(getpid());
    }
#endif
}

struct sTime
{
    std::chrono::time_point<std::chrono::steady_clock> m_start;

    void start()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    long long stopSeconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(end - m_start).count();
    }

    long long stopMilliseconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - m_start).count();
    }

    long long stopMicroseconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count();
    }

    long long stopNanoseconds()
    {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();
    }
};

struct sOutputManager
{
    bool m_isCout = true;
    bool m_isCerr = false;
    bool m_isFile = false;
    bool m_openCloseFileOnWrite = false;
    std::string m_filePath;
    std::ofstream m_file;

    void print(const std::string& msg)
    {
        if (m_isCout)
            std::cout << msg;
        if (m_isCerr)
            std::cerr << msg;
        if (m_isFile)
            printToFile(msg);
    }

    void colorPrint(ePrintColor color, const std::string& msg)
    {
        if (m_isCout)
            PrintInColor(color, msg, std::cout, false);
        if (m_isCerr)
            PrintInColor(color, msg, std::cout);
        if (m_isFile)
            printToFile(msg);
    }

    void resetFlags()
    {
        m_isCout = false;
        m_isCerr = false;
        m_isFile = false;
        m_openCloseFileOnWrite = false;
    }

private:
    void printToFile(const std::string& value)
    {
        if (!m_file.is_open())
            m_file.open(m_filePath, std::ios_base::app);

        if (m_openCloseFileOnWrite)
        {
            m_file << value;
            m_file.close();
        }
        else
            m_file << value;
    }
};

std::unordered_map<std::thread::id, std::unique_ptr<sTime>> Logger::m_upThreadsTimers;
std::unique_ptr<sOutputManager> Logger::m_upOutputter = std::make_unique<sOutputManager>();
bool Logger::m_useThreadID = false;
bool Logger::m_isOn = true;
std::mutex Logger::m_mtx;

void Logger::turnOff()
{
    m_isOn = false;
}

void Logger::turnOn()
{
    m_isOn = true;
}

void Logger::adjustSettings(int settingsFlags)
{
    m_upOutputter->resetFlags();
    m_useThreadID = false;

    if (settingsFlags & eLogSettings::UseCout)
        m_upOutputter->m_isCout = true;
    if (settingsFlags & eLogSettings::UseCerr)
        m_upOutputter->m_isCerr = true;
    if (settingsFlags & eLogSettings::UseFile)
        m_upOutputter->m_isFile = true;
    if (settingsFlags & eLogSettings::OpenCloseFile)
        m_upOutputter->m_openCloseFileOnWrite = true;
    if (settingsFlags & eLogSettings::ShowThreadID)
        m_useThreadID = true;
}

void Logger::setLogFilePath(const std::string& file, bool addProcessID)
{
    if (m_upOutputter->m_isFile)
    {
        std::string path = file;
        if (addProcessID)
        {
            const auto pos = path.find_last_of("/\\");
            if (pos != std::string::npos)
                path.insert(pos + 1, GetProcessID() + "_");
            else
                path.insert(0, GetProcessID() + "_");
        }
        m_upOutputter->m_filePath = path;
        m_upOutputter->m_file.open(path, std::ios_base::app);
    }
}

void Logger::print(const std::string& msg, eLogMsgType type)
{
    if (!m_isOn)
        return;
    std::lock_guard<std::mutex> lock(m_mtx);

    if (type == eLogMsgType::Info)
        m_upOutputter->colorPrint(ePrintColor::Green, "[Info]");
    else if (type == eLogMsgType::Warning)
        m_upOutputter->colorPrint(ePrintColor::Yellow, "[Warning]");
    else if (type == eLogMsgType::Error)
        m_upOutputter->colorPrint(ePrintColor::Red, "[ERROR]");

    if (m_useThreadID)
        printThreadID();
    m_upOutputter->print(type == eLogMsgType::None && !m_useThreadID
        ? msg + "\n" : ": " + msg + "\n");
}

void Logger::startTimer(const std::string& msg)
{
    if (!m_isOn)
        return;
    std::lock_guard<std::mutex> lock(m_mtx);

    startTimerForCurrThread();
    m_upOutputter->colorPrint(ePrintColor::Cyan, "[timer start]");
    if (m_useThreadID)
        printThreadID();
    m_upOutputter->print(": " + msg + "\n");
}

void Logger::stopTimer(eLogTimerUnits units, const std::string& msg)
{
    if (!m_isOn)
        return;
    std::lock_guard<std::mutex> lock(m_mtx);

    long long time = stopTimerForCurrThread(units);
    if (time == FAILED_TIME_MEASUREMENT)
        return;
    std::string unitsStr;
    if (units == eLogTimerUnits::Seconds)
        unitsStr = " sec";
    else if (units == eLogTimerUnits::Milliseconds)
        unitsStr = " millisec";
    else if (units == eLogTimerUnits::Microseconds)
        unitsStr = " microsec";
    else if (units == eLogTimerUnits::Nanoseconds)
        unitsStr = " nanosec";

    m_upOutputter->colorPrint(ePrintColor::Cyan, 
        "[timer stop " + std::to_string(time) + unitsStr + "]");
    if (m_useThreadID)
        printThreadID();
    m_upOutputter->print(": " + msg + "\n");
}

void Logger::startTimerForCurrThread()
{
    auto id = std::this_thread::get_id();
    if (m_upThreadsTimers.find(id) == m_upThreadsTimers.end())
        m_upThreadsTimers.insert(std::make_pair(id, std::make_unique<sTime>()));
    m_upThreadsTimers[id]->start();
}

long long Logger::stopTimerForCurrThread(eLogTimerUnits units)
{
    auto id = std::this_thread::get_id();
    if (m_upThreadsTimers.find(id) == m_upThreadsTimers.end())
        return FAILED_TIME_MEASUREMENT;

    long long res = 0;
    auto&& timer = m_upThreadsTimers[id];
    if (units == eLogTimerUnits::Seconds)
        res = timer->stopSeconds();
    if (units == eLogTimerUnits::Milliseconds)
        res = timer->stopMilliseconds();
    if (units == eLogTimerUnits::Microseconds)
        res = timer->stopMicroseconds();
    if (units == eLogTimerUnits::Nanoseconds)
        res = timer->stopNanoseconds();

    m_upThreadsTimers.erase(id);
    return res;
}

void Logger::printThreadID()
{
    std::ostringstream ss;
    ss << std::this_thread::get_id();
    m_upOutputter->colorPrint(ePrintColor::Magenta, "[thread " + ss.str() + "]");
}

void Logger::printObjectStr(const std::string& objStr)
{
    std::lock_guard<std::mutex> lock(m_mtx);
    m_upOutputter->print(objStr + "\n");
}
