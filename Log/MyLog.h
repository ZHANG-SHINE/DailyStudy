#ifndef MY_LOG_H
#define MY_LOG_H
#include <string>
#include <mutex>
#include <sstream>

namespace MySpace {
enum LogLevel {
    INFO = 0,
    DEBUG,
    ERROR
};

class MyLog {
public:
    static MyLog& GetInstance();

    void WriteLog(LogLevel level, const std::string& fileName,
                const std::string& funcName, int lineNum, const std::string& msg);

    MyLog& operator =(const MyLog&) = delete;

    MyLog(const MyLog&) = delete;

    ~MyLog() = default;
private:
    MyLog() = default;

    void Write(LogLevel level, const std::string& buff);

    int GetCurrentProcessId();

    std::string GetCurrentThreadId();

    std::string GetCurrentDateAndTime();

private:
    std::mutex lock_;
};

#define LOG_INFO(msg) MyLog::GetInstance().WriteLog(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__, msg);
#define LOG_DEBUG(msg) MyLog::GetInstance().WriteLog(LogLevel::DEBUG, __FILE__, __FUNCTION__, __LINE__, msg);
#define LOG_ERROR(msg) MyLog::GetInstance().WriteLog(LogLevel::ERROR, __FILE__, __FUNCTION__, __LINE__, msg);

} // namespace MySpace
#endif