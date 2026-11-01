#pragma once
#ifndef SCOREBWINDOW_H
#define SCOREBWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWizard>
#include <QtWidgets/QWizardPage>
#include <QtWidgets/QHeaderView>
#include <QtCore/QStringList>
#include <QtWidgets/QHeaderView>
#include <cstdlib>

#include <unordered_map>

#include "./globals.h"
#include "./utils.h"

class Scoreboard : public QWidget
{
    Q_OBJECT

public:
    Scoreboard(QWidget *parent = nullptr);
    ~Scoreboard();

    void clear();

private:
    QTableWidget *tableWidget;
    std::unordered_map<int, int> playerRowMap;

public:
    struct PlayerDTA
    {
        int id;
        QString name;
        int score;
        int ping;
    };

public slots:
    void updateTableSize(int playersCount);
    void handlePlayer(PlayerDTA player);
    bool removePlayer(int playerid);
};
#endif
