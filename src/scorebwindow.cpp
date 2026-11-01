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
    tableWidget->setRowCount(0);
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

void Scoreboard::clear()
{
    if (tableWidget)
    {
        tableWidget->setRowCount(0);
        playerRowMap.clear();
    }
}

void Scoreboard::updateTableSize(int playersCount)
{
    Q_UNUSED(playersCount);
    // Dense model: rows are dynamically added and removed by handlePlayer and removePlayer.
}

void Scoreboard::handlePlayer(PlayerDTA player)
{
    if (tableWidget == nullptr || player.id < 0)
        return;

    int row = -1;
    auto it = playerRowMap.find(player.id);
    if (it != playerRowMap.end())
    {
        row = it->second;
    }
    else
    {
        row = tableWidget->rowCount();
        tableWidget->insertRow(row);
        playerRowMap[player.id] = row;
    }

    auto updateOrCreateItem = [this, row](int col, const QString &text) {
        QTableWidgetItem *item = tableWidget->item(row, col);
        if (!item) {
            item = new QTableWidgetItem(text);
            tableWidget->setItem(row, col, item);
        } else {
            item->setText(text);
        }
    };

    updateOrCreateItem(0, QString::number(player.id));
    updateOrCreateItem(1, player.name);
    updateOrCreateItem(2, QString::number(player.score));
    updateOrCreateItem(3, QString::number(player.ping));
}

bool Scoreboard::removePlayer(int playerid)
{
    if (tableWidget == nullptr || playerid < 0)
        return false;

    auto it = playerRowMap.find(playerid);
    if (it == playerRowMap.end())
        return false;

    int removedRow = it->second;
    tableWidget->removeRow(removedRow);
    playerRowMap.erase(it);

    for (auto &pair : playerRowMap)
    {
        if (pair.second > removedRow)
            pair.second--;
    }
    return true;
}
