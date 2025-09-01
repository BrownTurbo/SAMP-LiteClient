#pragma once
#ifndef LOGSD_H
#define LOGSD_H

#include <time.h>
#include <memory>

#include <QtWidgets/QTextBrowser>
#include <QtCore/QSharedPointer>

#include "globals.h"

enum class LogLevel
{
    ERROR = 0,
    WARN = 1,
    DEBUG = 2,
    INFO = 4,
    FATAL = 8,
    VERBOSE = ((ERROR | WARN | DEBUG | INFO | FATAL) << 0x1)
};

const std::array<std::pair<LogLevel, std::pair<const char *, const char *>>, 6> LogLevelF = {
    std::make_pair(LogLevel::WARN, std::make_pair("WARNING", "warnings.log")),
    std::make_pair(LogLevel::ERROR, std::make_pair("ERROR", "errors.log")),
    std::make_pair(LogLevel::FATAL, std::make_pair("FATAL", "fatals.log")),
    std::make_pair(LogLevel::VERBOSE, std::make_pair("VERBOSE", "verbose.log")),
    std::make_pair(LogLevel::INFO, std::make_pair("INFO", "info.log")),
    std::make_pair(LogLevel::DEBUG, std::make_pair("DEBUG", "debug.log"))};

inline LogLevel operator|(LogLevel a, LogLevel b)
{
    return static_cast<LogLevel>(static_cast<int>(a) | static_cast<int>(b));
}
inline LogLevel &operator|=(LogLevel &in, LogLevel val)
{
    return in = in | val;
}
inline bool operator==(LogLevel a, int b)
{
    return static_cast<int>(a) == b;
}

class Logs : QObject
{
    Q_OBJECT

    public:
        void Log(LogLevel level, const char *fmt, ...);

    private slots:
        const char *GetLevelName(LogLevel level);
        const char *GetFileName(LogLevel level);
};
#endif
