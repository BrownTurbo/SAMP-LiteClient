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
#include <QtCore/QStringList>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtCore/QPointer>

#include "../lib/tinyxml/tinyxml2.h"
#include "./globals.h"
#include "./utils.h"

#include "../lib/query/QueryAPI.h"

#include <map>
#include <variant>
#include <vector>
#include <atomic>

#define MAX_SERVERS (150)

class ServersList : public QDialog
{
    Q_OBJECT

public:
    ServersList(QWidget *parent = nullptr);
    ~ServersList();

    struct srvDTA
    {
        std::string address;
        int port;
        std::string name;
        int ping = -1;
    };

    bool addServer(const srvDTA& server);
    bool removeServer(int serverid);

    void loadServers();
    void saveServers();

public slots:
    void onTick();

private slots:
    void onRemovePressed();
    void onAddPressed();
    void onSavePressed();
    void onCancelPressed();
    void onCellDoubleClicked(int row, int column);
    void updateServerRow(int serverID, const QString& name, int ping, bool online);

private:
    void queryServerAsync(int serverID, const std::string& address, int port);

    QTableWidget *serverL;
    QFrame *serverLine;
    QPushButton *SaveBtn;
    QPushButton *CancelBtn;
    QPushButton *AddBtn;
    QPushButton *RmvBtn;
    QLineEdit *lineEdit;
    QLabel *label;

    using ListValue = std::variant<std::string, int>;
    std::map<int, std::vector<ListValue>> srvData;

    int srvsCount = -1;
    std::atomic<bool> isQuerying{false};
    int tickCounter = 0;
};
#endif
