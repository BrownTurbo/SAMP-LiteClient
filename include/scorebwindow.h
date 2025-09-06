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

private slots:
    bool updateTableSize(int playersCount);

private:
    QTableWidget *tableWidget;

    struct PlayerDTA
    {
        int id;
        QString name;
        int score;
        int ping;
    };

public:
    bool isRowEmpty(int row);

    bool handlePlayer(PlayerDTA player);
};
#endif
