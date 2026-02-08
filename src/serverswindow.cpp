#include "../include/serverswindow.h"

ServersList::ServersList(QWidget *parent) : QDialog(parent)
{
    this->resize(800, 450);
    this->setFixedSize(800, 450);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ServersLWidget"));

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setGeometry(QRect(0, 0, 683, 352));
    VLayout->setContentsMargins(10, 10, 10, 10);
    VLayout->setSpacing(10);
    QHBoxLayout *HLayout_ = new QHBoxLayout(this);
    HLayout_->setGeometry(QRect(0, 1, 351, 348));
    QHBoxLayout *HLayout = new QHBoxLayout(this);
    HLayout_->setGeometry(QRect(0, 320, 300, 60));
    HLayout->addStretch();
    serverL = new QTableWidget(this);
    QFont font;
    font.setBold(true);
    font.setWeight(QFont::Weight::Bold);

    // Cols...
    const std::vector<QString> headers = {"ID", "Name", "Address", "Port", "Ping"};
    int colsCount = static_cast<int>(headers.size());
    if (serverL->columnCount() < colsCount)
        serverL->setColumnCount(colsCount);

    for (int i = 0; i < headers.size(); ++i) {
        auto* headerItem = new QTableWidgetItem(headers[i]);
        headerItem->setTextAlignment(Qt::AlignCenter);
        headerItem->setFont(font);

        serverL->setHorizontalHeaderItem(i, headerItem);
    }

    serverL->setObjectName(QString::fromUtf8("serverL"));
    serverL->setGeometry(QRect(0, 4, 450, 400));
    serverL->setFixedWidth(450);
    serverL->setFixedHeight(400);
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
    serverLine->setGeometry(QRect(302, 280, 371, 71));
    serverLine->setFrameShape(QFrame::HLine);
    serverLine->setFrameShadow(QFrame::Sunken);
    SaveBtn = new QPushButton(this);
    SaveBtn->setObjectName(QString::fromUtf8("SaveBtn"));
    SaveBtn->setGeometry(QRect(560, 320, 80, 30));
    SaveBtn->setFlat(false);
    SaveBtn->setText(QString::fromUtf8("Save"));
    SaveBtn->setCursor(QCursor(Qt::PointingHandCursor));
    AddBtn = new QPushButton(this);
    AddBtn->setObjectName(QString::fromUtf8("AddBtn"));
    AddBtn->setGeometry(QRect(130, 60, 80, 30));
    AddBtn->setFlat(false);
    AddBtn->setText(QString::fromUtf8("Add"));
    AddBtn->setCursor(QCursor(Qt::PointingHandCursor));
    RmvBtn = new QPushButton(this);
    RmvBtn->setObjectName(QString::fromUtf8("RmvBtn"));
    RmvBtn->setGeometry(QRect(130, 80, 80, 30));
    RmvBtn->setFlat(false);
    RmvBtn->setText(QString::fromUtf8("Remove"));
    RmvBtn->setCursor(QCursor(Qt::PointingHandCursor));
    CancelBtn = new QPushButton(this);
    CancelBtn->setObjectName(QString::fromUtf8("CancelBtn"));
    CancelBtn->setGeometry(QRect(470, 320, 80, 30));
    CancelBtn->setFlat(false);
    CancelBtn->setText(QString::fromUtf8("Cancel"));
    CancelBtn->setCursor(QCursor(Qt::PointingHandCursor));
    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setGeometry(QRect(50, 10, 200, 30));
    lineEdit->setFixedWidth(200);
    lineEdit->setFixedHeight(30);
    lineEdit->setText(QString::fromUtf8("127.0.0.1:7777"));
    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(35, 10, 91, 22));
    label->setFixedWidth(91);
    label->setFixedHeight(22);
    label->setText(QString::fromUtf8("IP Address:"));
    HLayout_->addWidget(serverL, 3, Qt::AlignmentFlag::AlignTop);
    HLayout_->addWidget(label, 1, Qt::AlignmentFlag::AlignTop);
    HLayout_->addWidget(lineEdit, 1, Qt::AlignmentFlag::AlignTop);
    HLayout_->addStretch();
    VLayout->addLayout(HLayout_);
    HLayout->addWidget(serverLine);
    HLayout->addWidget(RmvBtn);
    HLayout->addWidget(AddBtn);
    HLayout->addWidget(SaveBtn);
    HLayout->addWidget(CancelBtn);
    VLayout->addLayout(HLayout);
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
    //serverL->row()
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
