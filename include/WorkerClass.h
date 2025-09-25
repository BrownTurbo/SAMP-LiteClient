#pragma once
#ifndef WORKERCLASS_H
#define WORKERCLASS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QFlags>
#include <QtWidgets/QMessageBox>

#include <functional>

#include "utils.h"

class WorkerClass : public QObject
{
    Q_OBJECT

public:
    explicit WorkerClass(QObject *parent = nullptr) : QObject(parent ? (parent->thread() == QThread::currentThread() ? parent : nullptr) : nullptr) {
        this->connect(this, &WorkerClass::MessageBox, parent, &__MessageBox, Qt::QueuedConnection);
        this->connect(this, &WorkerClass::setStateMessage, parent, &__setStateMessage, Qt::QueuedConnection);
    }

signals:
    int MessageBox(const QString title, const QString message, QMessageBox::StandardButtons buttons, QMessageBox::Icon icon, QWidget *widget = nullptr, ZMessageBox action = ZMessageBox::Nothing, std::function<void(int)> callback = nullptr);
    bool setStateMessage(const char *text);
};

#endif // WORKERCLASS_H
