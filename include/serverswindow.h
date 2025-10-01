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

#define MAX_SERVERS 100
class ServersList : public QDialog
{
    Q_OBJECT

public:
    ServersList(QWidget *parent = nullptr);
    ~ServersList();

private:
    void onSavePressed();
    void onCancelPressed();

    QTableWidget *serverL;
    QFrame *serverLine;
    QPushButton *SaveBtn;
    QPushButton *CancelBtn;
    QLineEdit *lineEdit;
    QLabel *label;

    struct srvData
    {
        char *srvAddr;
        short srvPort;
        char *SRVName;
    };

    int srvCount;
    srvData srvList[MAX_SERVERS];
};
#endif
