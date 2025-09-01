#pragma once
#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>
#include <QtCore/QSharedPointer>

#include "globals.h"
//#include "utils.h"

class LogsWindow : public QDockWidget {

public:
    explicit LogsWindow(QWidget *parent = nullptr);
    ~LogsWindow();

private:
    QWidget *logsDockWidget;

public:
    QTextBrowser *logsBrowser;
};

#endif // LOGSWINDOW_H
