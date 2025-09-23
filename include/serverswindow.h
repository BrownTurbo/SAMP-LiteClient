#pragma once
#ifndef SERVERSLWINDOW_H
#define SERVERSLWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>

#include "./globals.h"

class ServersList : public QDialog
{
    Q_OBJECT

public:
    ServersList(QWidget *parent = nullptr);
    ~ServersList();

private:
    QTableWidget *serverL;
    QFrame *serverLine;
    QPushButton *SaveBtn;
    QPushButton *CancelBtn;
    QWidget *mainWidget;
    QLineEdit *lineEdit;
    QLabel *label;
};
#endif
