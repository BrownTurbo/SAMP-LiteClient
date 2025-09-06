#include "../include/scorebwindow.h"

Scoreboard::Scoreboard(QWidget *parent) : QDockWidget("Scoreboard", parent)
{
    QWidget *dockContent = new QWidget(this);

    dockContent->resize(1044, 707);
    dockContent->setFixedSize(1044, 707);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    dockContent->setSizePolicy(sizePolicy);
    if (dockContent->objectName().isEmpty())
        dockContent->setObjectName(QString::fromUtf8("ScoreBWidget"));
    tableWidget = new QTableWidget(this);
    if (tableWidget->objectName().isEmpty())
        tableWidget->setObjectName(QString::fromUtf8("scoreboard"));
    tableWidget->setRowCount(Globals::instance().getMaxPlayers());
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels({ "ID", "Name", "Score", "Ping" });
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->verticalHeader()->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(dockContent);
    layout->addWidget(tableWidget);
    dockContent->setLayout(layout);
    this->setWidget(dockContent);

    QMetaObject::connectSlotsByName(this);
}

Scoreboard::~Scoreboard() {}

bool Scoreboard::isRowEmpty(int row) {
    for (int col = 0; col < tableWidget->columnCount(); ++col)
    {
        QTableWidgetItem *item = tableWidget->item(row, col);
        if (item != nullptr && !item->text().isEmpty())
            return false;
    }
    return true;
}

bool Scoreboard::updateTableSize(int playersCount)
{
    tableWidget->setRowCount(playersCount);
    return true;
}

bool Scoreboard::handlePlayer(PlayerDTA player)
{
    for (int col = 0; col < tableWidget->columnCount(); ++col)
        tableWidget->takeItem(player.id, col);

    tableWidget->setItem(player.id, 0, new QTableWidgetItem(QString::number(player.id)));
    tableWidget->setItem(player.id, 1, new QTableWidgetItem(player.name));
    tableWidget->setItem(player.id, 2, new QTableWidgetItem(QString::number(player.score)));
    tableWidget->setItem(player.id, 3, new QTableWidgetItem(QString::number(player.ping)));
    return true;
}

bool Scoreboard::removePlayer(int playerid)
{
    for (int col = 0; col < tableWidget->columnCount(); ++col)
        tableWidget->takeItem(playerid, col);
    return true;
}
