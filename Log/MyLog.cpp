#include "MyLog.h"
#include <stdio.h>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <sstream>
#include <sys/time.h>

namespace MySpace {
// [时间][进程号 线程号][日志级别][文件名][方法名:行数] 日志信息
const std::string LOG_STR = "[%s][%d %s][%s][%s][%s:%d] %s\n";
const std::string TIME_STR = "%Y-%m-%d %H:%M:%S";
constexpr int MSG_BUFF_SIZE = 1024 * 2;
constexpr int MICROSEC_TO_MILLISEC = 1000;


MyLog& MyLog::GetInstance()
{
    static MyLog instance;
    return instance;
}

void MyLog::operator<(const LogStream& stream)
{
    Write(stream.GetLevel(), stream.GetStr());
}

std::string LogStream::GetCurrentDateAndTime() const
{
    struct timeval tv = {0};
    if (gettimeofday(&tv, nullptr) == -1) {
        return "TIME_ERROR";
    }
    char timeBuf[UINT8_MAX] = {0};
    struct tm result = {0};
    struct tm *tm = localtime_r(&tv.tv_sec, &result);
    if (tm == nullptr) {
        return "TIME_ERROR";
    }
    if (strftime(timeBuf, UINT8_MAX, TIME_STR.c_str(), tm) == 0) {
        return "TIME_ERROR";
    }
    return std::string(timeBuf) + "." + std::to_string(tv.tv_usec / MICROSEC_TO_MILLISEC);
}

int LogStream::GetCurrentProcessId() const
{
    auto pid = getpid();
    return pid;
}

std::string LogStream::GetCurrentThreadId() const
{
    auto thread_id = std::this_thread::get_id();
    std::stringstream ss;
    ss << thread_id;
    return ss.str();
}

void MyLog::Write(LogLevel level, const std::string& buf)
{
    std::lock_guard<std::mutex> lock(lock_);
    std::cout << buf;
}

const std::string LogStream::GetStr() const
{
    std::string levelStr = "INFO";
    if (level_ == LogLevel::DEBUG) {
        levelStr = "DEBUG";
    } else if (level_ == LogLevel::ERROR) {
        levelStr = "ERROR";
    }

    std::string time = GetCurrentDateAndTime();
    int pid = GetCurrentProcessId();
    std::string tid = GetCurrentThreadId();

    std::string fileName = fileName_.rfind('/') == std::string::npos ? fileName_ : fileName_.substr(fileName_.rfind('/') + 1);
    
    std::stringstream ss;
    ss << "["<< time << "][" << pid << " " << tid << "]["
        << levelStr << "][" << fileName << "][" << funcName_ << ":" << lineNum_ << "] " << ss_.str() << std::endl;
    return ss.str();
}
} // namespace MySpace
