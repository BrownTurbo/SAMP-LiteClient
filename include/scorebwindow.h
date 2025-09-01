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
#include <QtCore/QTimer>
#include <QtCore/QStringList>
#include <cstdlib>
#include <ctime>

#include "../globals.h"

class Scoreboard : public QDockWidget
{
    Q_OBJECT

public:
    Scoreboard(QWidget *parent = nullptr);
    ~Scoreboard();

/*private slots:
    void updatePings();
*/

public:
    void clearRow(QTableWidget *table, int row);
    bool isRowEmpty(QTableWidget *table, int row);

private:
    QTableWidget *tableWidget;
    QList<Player> players;

    void populatePlayers();

    struct Player
    {
        int id;
        QString name;
        int score;
        int ping;
    };
};
#endif
