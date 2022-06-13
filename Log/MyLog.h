#ifndef MY_LOG_H
#define MY_LOG_H
#include <string>
#include <mutex>
#include <sstream>
#include <iostream>

namespace MySpace {
enum LogLevel {
    INFO = 0,
    DEBUG,
    ERROR
};

class LogStream {
public:
    LogStream(LogLevel level, const std::string& fileName, const std::string& funcName, const int lineNum)
        : level_(level), fileName_(fileName), funcName_(funcName), lineNum_(lineNum) {}

    template <typename T>
    inline LogStream &operator<<(const T &val) noexcept
    {
        ss_ << val;
        return *this;
    }
    
    inline LogStream &operator<<(const uint8_t &val) noexcept
    {
        ss_ << static_cast<int>(val);
        return *this;
    }

    inline LogStream &operator<<(std::ostream& (*func)(std::ostream &os)) noexcept
    {
        ss_ << func;
        return *this;
    }

    const std::string GetStr() const;

    inline LogLevel GetLevel() const {return level_;}

    ~LogStream() = default;

private:
    int GetCurrentProcessId() const;

    std::string GetCurrentThreadId() const;

    std::string GetCurrentDateAndTime() const;

private:
    LogLevel level_;
    std::string fileName_;
    std::string funcName_;
    int lineNum_;
    std::stringstream ss_;
};

class MyLog {
public:
    static MyLog& GetInstance();

    void operator <(const LogStream& stream);

    MyLog& operator =(const MyLog&) = delete;

    MyLog(const MyLog&) = delete;

    ~MyLog() = default;
private:
    MyLog() = default;

    void Write(LogLevel level, const std::string& buff);

private:
    std::mutex lock_;
};

#define LOG_INFO MyLog::GetInstance() < LogStream(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__)
#define LOG_DEBUG MyLog::GetInstance() < LogStream(LogLevel::DEBUG, __FILE__, __FUNCTION__, __LINE__)
#define LOG_ERROR MyLog::GetInstance() < LogStream(LogLevel::ERROR, __FILE__, __FUNCTION__, __LINE__)

} // namespace MySpace
#endif
