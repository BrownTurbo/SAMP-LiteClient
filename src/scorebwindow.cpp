#include "../include/scorebwindow.h"

Scoreboard::Scoreboard(QWidget *parent = nullptr) : QDockWidget("Scoreboard", parent)
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

    //QTimer *timer = new QTimer(this);
    //connect(timer, &QTimer::timeout, this, &Scoreboard::updatePings);
    //timer->start(2000);
}

void populatePlayers()
{
    players = {
        {"Player1", 100, generateRandomPing()},
        {"Player2", 150, generateRandomPing()},
        {"Player3", 120, generateRandomPing()},
        {"Player4", 80, generateRandomPing()},
        {"Player5", 95, generateRandomPing()}};

    tableWidget->setRowCount(players.size());
    for (int i = 0; i < players.size(); ++i)
    {
        tableWidget->setItem(i, 0, new QTableWidgetItem(players[i].name));
        tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(players[i].score)));
        tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(players[i].ping)));
    }
}

Scoreboard::~Scoreboard() {}

void clearRow(QTableWidget *table, int row) {
    for (int col = 0; col < table->columnCount(); ++col)
        table->takeItem(row, col);
}

// Function to check if a row is empty
bool isRowEmpty(QTableWidget *table, int row) {
    for (int col = 0; col < table->columnCount(); ++col) {
        QTableWidgetItem *item = table->item(row, col);
        if (item != nullptr && !item->text().isEmpty())
            return false;
    }
    return true;
}
