#ifndef LOGGER_H
#define LOGGER_H

#include <string>

class Logger {
public:
    static void info(const std::string& message);
    static void error(const std::string& message);
    static void debug(const std::string& message);
private:
    static std::string getCurrentTime();
    static void printLog(const std::string& level, const std::string& message);
};

#endif // LOGGER_H