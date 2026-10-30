#include "../include/serverswindow.h"

ServersList::ServersList(QWidget *parent) : QDialog(parent)
{
    this->resize(800, 480);
    this->setFixedSize(800, 480);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ServersLWidget"));

    serverL = new QTableWidget(this);
    QFont font;
    font.setBold(true);
    font.setWeight(QFont::Weight::Bold);

    // Cols...
    const std::vector<QString> headers = {"ID", "Name", "Address", "Port", "Ping"};
    int colsCount = static_cast<int>(headers.size());
    if (serverL->columnCount() < colsCount)
        serverL->setColumnCount(colsCount);

    for (int i = 0; i < (int)headers.size(); ++i) {
        auto* headerItem = new QTableWidgetItem(headers[i]);
        headerItem->setTextAlignment(Qt::AlignCenter);
        headerItem->setFont(font);
        serverL->setHorizontalHeaderItem(i, headerItem);
    }

    serverL->setObjectName(QString::fromUtf8("serverL"));
    serverL->setFrameShape(QFrame::Box);
    serverL->setFrameShadow(QFrame::Raised);
    serverL->setLineWidth(1);
    serverL->setMidLineWidth(0);
    serverL->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    serverL->horizontalHeader()->setCascadingSectionResizes(false);
    serverL->setAutoScroll(true);
    serverL->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    serverL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    serverL->setSelectionBehavior(QAbstractItemView::SelectRows);
    serverL->setSelectionMode(QAbstractItemView::SingleSelection);
    serverL->verticalHeader()->setVisible(false);
    serverL->setRowCount(MAX_SERVERS);

    serverLine = new QFrame(this);
    serverLine->setObjectName(QString::fromUtf8("serverLine"));
    serverLine->setFrameShape(QFrame::HLine);
    serverLine->setFrameShadow(QFrame::Sunken);

    SaveBtn = new QPushButton(this);
    SaveBtn->setObjectName(QString::fromUtf8("SaveBtn"));
    SaveBtn->setFlat(false);
    SaveBtn->setText(QString::fromUtf8("Save"));
    SaveBtn->setCursor(QCursor(Qt::PointingHandCursor));
    SaveBtn->setFixedWidth(80);

    AddBtn = new QPushButton(this);
    AddBtn->setObjectName(QString::fromUtf8("AddBtn"));
    AddBtn->setFlat(false);
    AddBtn->setText(QString::fromUtf8("Add"));
    AddBtn->setCursor(QCursor(Qt::PointingHandCursor));
    AddBtn->setFixedWidth(80);

    RmvBtn = new QPushButton(this);
    RmvBtn->setObjectName(QString::fromUtf8("RmvBtn"));
    RmvBtn->setFlat(false);
    RmvBtn->setText(QString::fromUtf8("Remove"));
    RmvBtn->setCursor(QCursor(Qt::PointingHandCursor));
    RmvBtn->setFixedWidth(80);

    CancelBtn = new QPushButton(this);
    CancelBtn->setObjectName(QString::fromUtf8("CancelBtn"));
    CancelBtn->setFlat(false);
    CancelBtn->setText(QString::fromUtf8("Cancel"));
    CancelBtn->setCursor(QCursor(Qt::PointingHandCursor));
    CancelBtn->setFixedWidth(80);

    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setText(QString::fromUtf8("127.0.0.1:7777"));

    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setText(QString::fromUtf8("IP Address:"));

    // ── Right panel: IP input row + Add/Remove row + stretch ──
    QVBoxLayout *rightPanel = new QVBoxLayout();
    rightPanel->setSpacing(8);
    rightPanel->setContentsMargins(0, 0, 0, 0);

    // Row: "IP Address:" label + lineEdit
    QHBoxLayout *ipRow = new QHBoxLayout();
    ipRow->setSpacing(6);
    ipRow->addWidget(label);
    ipRow->addWidget(lineEdit, 1);

    // Row: Remove + Add buttons (right-aligned)
    QHBoxLayout *addRemoveRow = new QHBoxLayout();
    addRemoveRow->setSpacing(6);
    addRemoveRow->addStretch();
    addRemoveRow->addWidget(RmvBtn);
    addRemoveRow->addWidget(AddBtn);

    rightPanel->addLayout(ipRow);
    rightPanel->addLayout(addRemoveRow);
    rightPanel->addStretch();

    // ── Main content row: table | right panel ──
    QHBoxLayout *contentRow = new QHBoxLayout();
    contentRow->setSpacing(10);
    contentRow->addWidget(serverL, 1);
    contentRow->addLayout(rightPanel);

    // ── Bottom row: separator line + Save/Cancel ──
    QHBoxLayout *bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(6);
    bottomRow->addWidget(serverLine, 1);
    bottomRow->addWidget(SaveBtn);
    bottomRow->addWidget(CancelBtn);

    // ── Root vertical layout ──
    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setContentsMargins(10, 10, 10, 10);
    VLayout->setSpacing(10);
    VLayout->addLayout(contentRow, 1);
    VLayout->addLayout(bottomRow);
    this->setLayout(VLayout);

    this->connect(SaveBtn, &QPushButton::clicked, this, &ServersList::onSavePressed);
    this->connect(CancelBtn, &QPushButton::clicked, this, &ServersList::onCancelPressed);
    this->connect(AddBtn, &QPushButton::clicked, this, &ServersList::onAddPressed);
    this->connect(RmvBtn, &QPushButton::clicked, this, &ServersList::onRemovePressed);

    QMetaObject::connectSlotsByName(this);
}

ServersList::~ServersList() {}

void ServersList::onAddPressed()
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    srvDTA DTA;
    QString AddrTXT = lineEdit->text();
    if (AddrTXT.isEmpty() || AddrTXT.isNull() || !AddrTXT.contains(':') || (AddrTXT.contains(':') && AddrTXT.count(':') > 1))
    {
        emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Invalid IP Address."), QMessageBox::Ok, QMessageBox::Information);
        return;
    }
    QStringList AddrParts = AddrTXT.split(':');

    DTA.address = AddrParts.at(0).toUtf8();
    DTA.port = AddrParts.at(1).toInt();

    if(!ServersList::addServer(DTA))
    {
        emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Something went wrong."), QMessageBox::Ok, QMessageBox::Information);
        return;
    }
    emit worker.MessageBox(QString::fromUtf8("Success"), QString::fromUtf8("[SAMP] Server is added successfully."), QMessageBox::Ok, QMessageBox::Information);
}

void ServersList::onRemovePressed()
{
    int row = serverL->currentRow();
    if (row < 0)
        return;
    removeServer(row);
}

void ServersList::onSavePressed()
{
    //
}

void ServersList::onCancelPressed()
{
    this->close();
}

bool ServersList::addServer(const srvDTA& server)
{
    if (serverL == nullptr)
        return false;
    if (serverL->isHidden())
        return false;

    int serverID = -1;
    serverID = ++srvsCount;
    if (serverID >= MAX_SERVERS)
        return false;

    if (serverID < 0 || serverID >= serverL->rowCount())
        return false;

    if (!isTableWidgetRowEmpty(serverL, serverID))
    {
        for (int col = 0; col < serverL->columnCount(); ++col)
            serverL->takeItem(serverID, col);
    }

    QueryAPI::SAMPServerInfo serverInfo;
    bool success = false;

    QueryAPI::GetSAMPServerInfo(server.address.c_str(), server.port, serverInfo, success);
    if (success) {
        serverL->setItem(serverID, 0, new QTableWidgetItem(QString::number(serverID)));
        serverL->setItem(serverID, 1, new QTableWidgetItem(QString::fromUtf8(serverInfo.serverName)));
        serverL->setItem(serverID, 2, new QTableWidgetItem(QString::fromUtf8(server.address.c_str())));
        serverL->setItem(serverID, 3, new QTableWidgetItem(QString::number(server.port)));
        serverL->setItem(serverID, 4, new QTableWidgetItem(QString::number(serverInfo.ping)));

        srvData[serverID] = std::vector<ListValue>{
            server.address,
            server.port
        };
        return true;
    }
    else
        return false;
}

bool ServersList::removeServer(int serverid)
{
    if (serverL == nullptr)
        return false;
    if (serverL->isHidden())
        return false;
    if (serverid < 0 || serverid >= serverL->rowCount())
        return false;

    if (isTableWidgetRowEmpty(serverL, serverid))
        return false;

    if (serverL->item(serverid, 0)->text().toInt() == serverid)
    {
        for (int col = 0; col < serverL->columnCount(); ++col)
            serverL->takeItem(serverid, col);
    }

    srvData.erase(serverid);
    return true;
}

void ServersList::onTick()
{
    QueryAPI::SAMPServerInfo serverInfo;
    bool success = false;
    for (auto& [serverID, serverDTA] : srvData)
    {
        std::string addr = std::get<std::string>(serverDTA[0]);
        int port = std::get<int>(serverDTA[1]);
        QueryAPI::GetSAMPServerInfo(addr.c_str(), port, serverInfo, success);
        if (success)
        {
            serverL->setItem(serverID, 0, new QTableWidgetItem(QString::number(serverID)));
            serverL->setItem(serverID, 1, new QTableWidgetItem(QString::fromUtf8(serverInfo.serverName)));
            serverL->setItem(serverID, 2, new QTableWidgetItem(QString::fromUtf8(addr)));
            serverL->setItem(serverID, 3, new QTableWidgetItem(QString::number(port)));
            serverL->setItem(serverID, 4, new QTableWidgetItem(QString::number(serverInfo.ping)));
        }
        else
        {
            serverL->setItem(serverID, 4, new QTableWidgetItem("<OFFLINE>"));
        }
    }
}
