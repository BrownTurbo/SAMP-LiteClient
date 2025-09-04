#include "../include/logs.h"

const char *Logs::GetLevelName(LogLevel level)
{
    char *levelName = NULL;
    for (auto iter = LogLevelF.begin(); iter != LogLevelF.end(); ++iter)
    {
        if (static_cast<LogLevel>(iter->first) != level)
            continue;
        levelName = const_cast<char *>((iter->second).first);
    }
    if (levelName == NULL)
        levelName = const_cast<char *>("<unknown>");
    return levelName;
}

const char *Logs::GetFileName(LogLevel level)
{
    char *fileN = NULL;
    for (auto iter = LogLevelF.begin(); iter != LogLevelF.end(); ++iter)
    {
        if (static_cast<LogLevel>(iter->first) != level)
            continue;
        fileN = const_cast<char *>((iter->second).second);
    }
    if (fileN == NULL)
        fileN = const_cast<char *>("unknown.log");
    return fileN;
}

void Logs::Log(LogLevel level, const char *fmt, ...)
{
    char buffer[1024 + 1];
    memset(buffer, 0, sizeof(buffer));

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    QSharedPointer<QTextBrowser> _logsBrowser = Globals::instance().getLogsD();
    QTextBrowser* logsBrowser = _logsBrowser.data();
    QWidget* logsWidget = Globals::instance().getLogsW();
    struct tm * st;
    time_t curr_time = time(NULL);
    st = localtime(&curr_time);

    QMetaObject::invokeMethod(logsWidget, [=,this]() {
        logsBrowser->append("<b> " + QString::fromUtf8(const_cast<char *>(GetLevelName(level))) + ";" + QString::number(st->tm_year + 1900) + "-" + QString::number(st->tm_mon + 1) + "-" + QString::number(st->tm_mday) + " " + QString::number(st->tm_hour) + ":" + QString::number(st->tm_min) + ":" + QString::number(st->tm_sec) + "</b> " + QString::fromUtf8(buffer));
    });
}
