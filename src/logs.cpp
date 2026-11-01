#include "../include/logs.h"
#include <mutex>
#include <vector>

static std::mutex s_logsMutex;
static std::vector<QString> s_pendingLogs;

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

void Logs::flushPendingLogs()
{
    std::lock_guard<std::mutex> lock(s_logsMutex);
    if (!Globals::instance().hasLogs() || s_pendingLogs.empty())
        return;

    QSharedPointer<QTextBrowser> _logsBrowser = Globals::instance().getLogsD();
    QTextBrowser* logsBrowser = _logsBrowser.data();
    QWidget* logsWidget = Globals::instance().getLogsW();
    if (!logsWidget || !logsBrowser)
        return;

    std::vector<QString> toFlush = std::move(s_pendingLogs);
    s_pendingLogs.clear();

    QMetaObject::invokeMethod(logsWidget, [logsBrowser, toFlush]() {
        if (logsBrowser) {
            for (const auto& logMsg : toFlush) {
                logsBrowser->append(logMsg);
            }
        }
    });
}

void Logs::Log(LogLevel level, const char *fmt, ...)
{
    char buffer[1024 + 1];
    memset(buffer, 0, sizeof(buffer));

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    struct tm st;
    time_t curr_time = time(NULL);
    struct tm *st_ptr = localtime(&curr_time);
    if (st_ptr)
        st = *st_ptr;
    else
        memset(&st, 0, sizeof(st));

    std::string levelStr(GetLevelName(level));

    const char *fileName = GetFileName(level);
    if (fileName && ROOT_PATH && strlen(ROOT_PATH) > 0)
    {
        char filePath[256];
        snprintf(filePath, sizeof(filePath), "%s/logs/%s", ROOT_PATH, fileName);
        FILE *f = fopen(filePath, "a");
        if (f)
        {
            fprintf(f, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
                st.tm_year + 1900, st.tm_mon + 1, st.tm_mday,
                st.tm_hour, st.tm_min, st.tm_sec,
                levelStr.c_str(), buffer);
            fclose(f);
        }
    }

    QString colorCode = "#888888";
    if (level == LogLevel::ERROR || level == LogLevel::FATAL)
        colorCode = "#FF4444";
    else if (level == LogLevel::WARN)
        colorCode = "#FFAA00";
    else if (level == LogLevel::INFO)
        colorCode = "#4B9EFF";

    char timeBuf[64];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d:%02d", st.tm_hour, st.tm_min, st.tm_sec);

    QString htmlMessage = QString("<font color=\"#888888\">[%1]</font> <font color=\"%2\"><b>[%3]</b></font> %4")
        .arg(timeBuf)
        .arg(colorCode)
        .arg(QString::fromUtf8(levelStr.c_str()))
        .arg(QString::fromUtf8(buffer).toHtmlEscaped());

    std::lock_guard<std::mutex> lock(s_logsMutex);
    if (!Globals::instance().hasLogs())
    {
        s_pendingLogs.push_back(htmlMessage);
        return;
    }

    QSharedPointer<QTextBrowser> _logsBrowser = Globals::instance().getLogsD();
    QTextBrowser* logsBrowser = _logsBrowser.data();
    QWidget* logsWidget = Globals::instance().getLogsW();

    if (!logsWidget || !logsBrowser)
    {
        s_pendingLogs.push_back(htmlMessage);
        return;
    }

    std::vector<QString> toFlush;
    if (!s_pendingLogs.empty())
    {
        toFlush = std::move(s_pendingLogs);
        s_pendingLogs.clear();
    }

    QMetaObject::invokeMethod(logsWidget, [logsBrowser, toFlush, htmlMessage]() {
        if (logsBrowser) {
            for (const auto& pending : toFlush)
                logsBrowser->append(pending);
            logsBrowser->append(htmlMessage);
        }
    });
}
