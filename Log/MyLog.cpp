#include "MyLog.h"
#include <stdio.h>
#include <chrono>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <sstream>

namespace MySpace {
// [时间][进程号 线程号][日志级别][文件名][方法名:行数] 日志信息
const std::string LOG_STR = "[%s][%d %s][%s][%s][%s:%d] %s\n";
const std::string TIME_STR = "%Y-%m-%d %H:%M:%S";

constexpr int MSG_BUFF_SIZE = 1024 * 2;

MyLog& MyLog::GetInstance()
{
    static MyLog instance;
    return instance;
}

void MyLog::WriteLog(LogLevel level, const std::string& fileName, const std::string& funcName, int lineNum, const std::string& msg) 
{
    std::string levelStr = "INFO";
    if (level == LogLevel::DEBUG) {
        levelStr = "DEBUG";
    } else if (level == LogLevel::ERROR) {
        levelStr = "ERROR";
    }

    std::string time = GetCurrentDateAndTime();
    int pid = GetCurrentProcessId();
    std::string tid = GetCurrentThreadId();
    
    std::stringstream ss;
    ss << "["<< time << "][" << pid << " " << tid << "]["
        << levelStr << "][" << fileName.substr(fileName.rfind('/') + 1) << "][" << funcName
        << ":" << lineNum << "] " << msg << std::endl;
    Write(level, ss.str());
}

std::string MyLog::GetCurrentDateAndTime()
{
    std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
    time_t tm = std::chrono::system_clock::to_time_t(today);
    struct tm info;
    localtime_r(&tm, &info);
    char buff[80];
    strftime(buff, sizeof(buff), TIME_STR.c_str(), &info);
    return buff;
}

int MyLog::GetCurrentProcessId()
{
    auto pid = getpid();
    return pid;
}

std::string MyLog::GetCurrentThreadId()
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
} // namespace MySpace