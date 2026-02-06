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
#include "./utils.h"

#include "../lib/query/QueryAPI.h"

#include <map>
#include <variant>
#include <vector>

#define MAX_SERVERS (150)

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

    struct srvDTA
    {
        std::string address;
        int port;
    };

    using ListValue = std::variant<std::string, int>;
    std::map<int, std::vector<ListValue>> srvData;

    int srvsCount = -1;

public:
    bool addServer(const srvDTA& server);
    bool removeServer(int serverid);

    void onTick();
};
#endif
