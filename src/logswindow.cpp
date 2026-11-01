#include "logswindow.h"
#include "logs.h"

LogsWindow::LogsWindow(QWidget *parent) : QWidget(parent) {
        if (this->objectName().isEmpty())
            this->setObjectName(QString::fromUtf8("LogsWidget"));

        this->resize(775, 337);
        this->setFixedSize(QSize(775, 337));
        logsBrowser = new QTextBrowser(this);
        logsBrowser->setObjectName(QString::fromUtf8("logsBrowser"));
        logsBrowser->setGeometry(QRect(0, 0, 775, 337));
        logsBrowser->resize(QSize(775, 337));
        logsBrowser->setFixedSize(QSize(775, 337));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(logsBrowser->sizePolicy().hasHeightForWidth());
        logsBrowser->setSizePolicy(sizePolicy);

        logsBrowser->setReadOnly(true);

        this->setWindowTitle(QString::fromUtf8("Logs"));

        QPointer<QTextBrowser> guarded(logsBrowser);
        Globals::instance().setLogsD(QSharedPointer<QTextBrowser>(logsBrowser, [guarded](QTextBrowser* ptr) {
            if (guarded && ptr && !guarded->parent())
                ptr->deleteLater();
        }));
        Globals::instance().setLogsW(this);
        Logs::flushPendingLogs();

        QMetaObject::connectSlotsByName(this);
}

LogsWindow::~LogsWindow() {
    Globals::instance().setLogsD(nullptr);
    Globals::instance().setLogsW(nullptr);
}
