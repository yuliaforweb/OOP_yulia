#include "logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

string Logger::getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto in_time_t = chrono::system_clock::to_time_t(now);

    tm timeInfo;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&timeInfo, &in_time_t);
#else
    localtime_r(&in_time_t, &timeInfo);
#endif

    stringstream ss;
    ss << put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::printLog(const string& level, const string& message) {
    cout << "[" << getCurrentTime() << "] [" << level << "] " << message << endl;
}

void Logger::info(const string& message) {
    printLog("INFO", message);
}

void Logger::error(const string& message) {
    printLog("ERROR", message);
}

void Logger::debug(const string& message) {
    printLog("DEBUG", message);
}