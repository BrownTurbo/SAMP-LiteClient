#pragma once
#ifndef SCOREBWINDOW_H
#define SCOREBWINDOW_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWizard>
#include <QtWidgets/QWizardPage>
#include <QtWidgets/QHeaderView>
#include <QtCore/QStringList>
#include <cstdlib>

#include "./globals.h"

class Scoreboard : public QDockWidget
{
    Q_OBJECT

public:
    Scoreboard(QWidget *parent = nullptr);
    ~Scoreboard();

private:
    QTableWidget *tableWidget;

    bool isRowEmpty(int row);

public:
    struct PlayerDTA
    {
        int id;
        QString name;
        int score;
        int ping;
    };

    void updateTableSize(int playersCount);
    void handlePlayer(PlayerDTA player);
    bool removePlayer(int playerid);
};
#endif
