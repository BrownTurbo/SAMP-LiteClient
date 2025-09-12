#include "../include/scorebwindow.h"

Scoreboard::Scoreboard(QWidget *parent) : QWidget(parent)
{
    this->resize(1044, 707);
    this->setFixedSize(1044, 707);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ScoreBWidget"));
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

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tableWidget);
    this->setLayout(layout);

    QMetaObject::connectSlotsByName(this);
}

Scoreboard::~Scoreboard() {}

bool Scoreboard::isRowEmpty(int row) {
    if(tableWidget == nullptr)
        return false;

    if (tableWidget->columnCount() == 0)
        return false;

    for (int col = 0; col < tableWidget->columnCount(); ++col)
    {
        QTableWidgetItem *item = tableWidget->item(row, col);
        if (item != nullptr && !item->text().isEmpty())
            return false;
    }
    return true;
}

void Scoreboard::updateTableSize(int playersCount)
{
    if (tableWidget == nullptr)
        return;

    tableWidget->setRowCount(playersCount);
}

void Scoreboard::handlePlayer(PlayerDTA player)
{
    if (tableWidget == nullptr)
        return;

    if (!isRowEmpty(player.id))
    {
        for (int col = 0; col < tableWidget->columnCount(); ++col)
            tableWidget->takeItem(player.id, col);
    }

    tableWidget->setItem(player.id, 0, new QTableWidgetItem(QString::number(player.id)));
    tableWidget->setItem(player.id, 1, new QTableWidgetItem(player.name));
    tableWidget->setItem(player.id, 2, new QTableWidgetItem(QString::number(player.score)));
    tableWidget->setItem(player.id, 3, new QTableWidgetItem(QString::number(player.ping)));
}

bool Scoreboard::removePlayer(int playerid)
{
    if (tableWidget == nullptr)
        return false;

    if (isRowEmpty(playerid))
        return false;

    for (int col = 0; col < tableWidget->columnCount(); ++col)
        tableWidget->takeItem(playerid, col);
    return true;
}
