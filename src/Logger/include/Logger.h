/**
 * @file logger.h
 * @brief Header file for the Logger class.
 */

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>

struct sTime;
struct sOutputManager;

/**
 * @brief Enumerations for time units of timer.
 */
enum class eLogTimerUnits : uint8_t
{
    Seconds,         //!< Seconds
    Milliseconds,    //!< Milliseconds
    Microseconds,    //!< Microseconds
    Nanoseconds      //!< Nanoseconds
};

/**
 * @brief Enumerations for logger message type.
 */
enum class eLogMsgType : uint8_t
{
    None,      //!< None
    Info,      //!< Info
    Warning,   //!< Warning
    Error      //!< Error
};

/**
 * @brief Flags for logger settings.
 */
enum eLogSettings : uint8_t
{
    UseCout = 1 << 0,     //!< Use standard output (cout)
    UseCerr = 1 << 1,     //!< Use standard error output (cerr)
    UseFile = 1 << 2,     //!< Use file output
    ShowThreadID = 1 << 3,//!< Show thread ID in log messages
    OpenCloseFile = 1 << 4//!< Use file open-close strategy for each writing
};

/**
 * @brief A static class for logging messages and measuring time intervals.
 * @note By defult is enebled and use std::cout as default output
 */
class Logger
{
    Logger() = delete;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

private:
    /**
     * @brief Map to store timers for each thread.
     */
    static std::unordered_map<std::thread::id, std::unique_ptr<sTime>> m_upThreadsTimers;

    /**
     * @brief Pointer to the outputter responsible for displaying or writing logs.
     */
    static std::unique_ptr<sOutputManager> m_upOutputter;

    /**
     * @brief Flag indicating whether to include thread IDs in log messages.
     */
    static bool m_useThreadID;

    /**
     * @brief Flag indicating whether the logger is turned on or off.
     */
    static bool m_isOn;

    /**
     * @brief Mutex for thread-safe operations.
     */
    static std::mutex m_mtx;

public:
    /**
     * @brief Turns off the logger.
     * @note By default logger is enabled.
     */
    static void turnOff();

    /**
     * @brief Turns on the logger.
     * @note By default logger is enabled.
     */
    static void turnOn();

    /**
     * @brief Adjusts the logger settings based on the provided flags.
     * @param settingsFlags The flags specifying the settings to adjust.
     * @note If flag eLogSettings::UseFile is setted but logger file path is not provided then there will no log file.
     */
    static void adjustSettings(int settingsFlags);

    /**
     * @brief Sets the file path for logging if file output is enabled.
     * @param file The file path for logging.
     * @param addProcessID Adds the process ID to the file name. It helps to generate different logs for different runs.
     * @note This method have no affect if the eLogSettings::UseFile is not setted
     */
    static void setLogFilePath(const std::string& file, bool addProcessID = true);

    /**
     * @brief Prints a log message with optional message type.
     * @param msg The message to be logged.
     * @param type The type of the log message.
     */
    static void print(const std::string& msg, eLogMsgType type = eLogMsgType::None);

    /**
     * @brief Starts a timer with the specified message.
     * @param msg The message associated with the timer.
     */
    static void startTimer(const std::string& msg);

    /**
     * @brief Stops a timer and prints the elapsed time.
     * @param units The units in which to display the elapsed time.
     * @param msg The message associated with the timer.
     */
    static void stopTimer(eLogTimerUnits units, const std::string& msg);

private:
    /**
     * @brief Starts a timer for the current thread.
     */
    static void startTimerForCurrThread();

    /**
     * @brief Stops the timer for the current thread and returns the elapsed time.
     * @param units The units in which to return the elapsed time.
     */
    static long long stopTimerForCurrThread(eLogTimerUnits units);

    /**
     * @brief Prints the current thread ID.
     */
    static void printThreadID();

    /**
     * @brief Prints a string representation of an object.
     * @param objStr The string representation of the object.
     */
    static void printObjectStr(const std::string& objStr);

public:
    /**
     * @brief Prints a string representation of an object.
     * @tparam T The type of the object.
     * @param obj The object to be printed.
     * @note This method assumes that there exists a typeToString friend function that can convert the object to a string.
     */
    template<typename T>
    static void printObject(const T& obj)
    {
        printObjectStr(typeToString(obj));
    }

    /**
     * @brief Prints a formatted log message with optional message type.
     * @tparam Args Variadic template parameter pack for formatting arguments.
     * @param type The type of the log message.
     * @param format The format string for printing.
     * @param args Variadic arguments to be formatted and printed.
     */
    template <typename... Args>
    static void print(eLogMsgType type, const char* format, Args... args)
    {
        char buff[1024];
        std::snprintf(buff, 1024, format, args...);
        print(buff, type);
    }
};