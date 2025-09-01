#include "logswindow.h"

LogsWindow::LogsWindow(QWidget *parent) : QDockWidget(parent) {
        if (this->objectName().isEmpty())
            this->setObjectName(QString::fromUtf8("LogsWidget"));

        this->resize(775, 337);
        this->setFixedSize(QSize(775, 337));
        logsDockWidget = new QWidget();
        logsDockWidget->setObjectName(QString::fromUtf8("logsDockWidget"));
        logsDockWidget->setEnabled(true);
        logsBrowser = new QTextBrowser(logsDockWidget);
        logsBrowser->setObjectName(QString::fromUtf8("logsBrowser"));
        logsBrowser->setGeometry(QRect(0, 0, 775, 337));
        logsBrowser->resize(QSize(775, 337));
        logsBrowser->setFixedSize(QSize(775, 337));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(logsBrowser->sizePolicy().hasHeightForWidth());
        logsBrowser->setSizePolicy(sizePolicy);
        this->setWidget(logsDockWidget);

        logsBrowser->setReadOnly(true);

        this->setWindowTitle(QString::fromUtf8("Logs"));

        Globals::instance().setLogsD(QSharedPointer<QTextBrowser>(logsBrowser, [](QTextBrowser* ptr) {
            if (ptr && !ptr->parent())
                ptr->deleteLater();
        }));
        Globals::instance().setLogsW(QSharedPointer<QDockWidget>(this, [](QDockWidget* ptr) {
            if (ptr && !ptr->parent())
                ptr->deleteLater();
        }));

        QMetaObject::connectSlotsByName(this);
}

LogsWindow::~LogsWindow() {}
