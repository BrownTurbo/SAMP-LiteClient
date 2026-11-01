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

    this->connect(serverL, &QTableWidget::cellDoubleClicked, this, &ServersList::onCellDoubleClicked);
    this->connect(SaveBtn, &QPushButton::clicked, this, &ServersList::onSavePressed);
    this->connect(CancelBtn, &QPushButton::clicked, this, &ServersList::onCancelPressed);
    this->connect(AddBtn, &QPushButton::clicked, this, &ServersList::onAddPressed);
    this->connect(RmvBtn, &QPushButton::clicked, this, &ServersList::onRemovePressed);

    loadServers();

    QMetaObject::connectSlotsByName(this);
}

ServersList::~ServersList() {}

void ServersList::onAddPressed()
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    srvDTA DTA;
    QString AddrTXT = lineEdit->text().trimmed();
    if (AddrTXT.isEmpty() || AddrTXT.isNull() || !AddrTXT.contains(':') || (AddrTXT.contains(':') && AddrTXT.count(':') > 1))
    {
        emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Invalid IP Address format (use ip:port)."), QMessageBox::Ok, QMessageBox::Information);
        return;
    }
    QStringList AddrParts = AddrTXT.split(':');

    DTA.address = AddrParts.at(0).toUtf8().constData();
    DTA.port = AddrParts.at(1).toInt();
    if (DTA.port <= 0 || DTA.port > 65535)
    {
        emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Invalid Port number."), QMessageBox::Ok, QMessageBox::Information);
        return;
    }

    if (!ServersList::addServer(DTA))
    {
        emit worker.MessageBox(QString::fromUtf8("Error"), QString::fromUtf8("[SAMP] Server limit reached or could not add."), QMessageBox::Ok, QMessageBox::Information);
        return;
    }
    emit worker.MessageBox(QString::fromUtf8("Success"), QString::fromUtf8("[SAMP] Server added and querying started."), QMessageBox::Ok, QMessageBox::Information);
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
    saveServers();
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    emit worker.MessageBox(QString::fromUtf8("Success"), QString::fromUtf8("[SAMP] Servers saved successfully."), QMessageBox::Ok, QMessageBox::Information);
}

void ServersList::onCancelPressed()
{
    this->close();
}

void ServersList::onCellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    if (srvData.find(row) == srvData.end())
        return;

    std::string addr = std::get<std::string>(srvData[row][0]);
    int port = std::get<int>(srvData[row][1]);

    strncpy(settings.server.szAddr, addr.c_str(), sizeof(settings.server.szAddr) - 1);
    settings.server.szAddr[sizeof(settings.server.szAddr) - 1] = '\0';
    settings.server.iPort = port;

    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    emit worker.MessageBox(QString::fromUtf8("Server Selected"),
        QString("[SAMP] Server %1:%2 selected for connection.").arg(QString::fromUtf8(addr.c_str())).arg(port),
        QMessageBox::Ok, QMessageBox::Information);
}

void ServersList::saveServers()
{
    QString xmlPath = GetPath("servers.xml");
    if (xmlPath.isEmpty())
        return;

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLElement* root = doc.NewElement("servers");
    doc.InsertFirstChild(root);

    for (const auto& [id, srv] : srvData)
    {
        if (srv.size() >= 2)
        {
            tinyxml2::XMLElement* elem = doc.NewElement("server");
            elem->SetAttribute("address", std::get<std::string>(srv[0]).c_str());
            elem->SetAttribute("port", std::get<int>(srv[1]));
            root->InsertEndChild(elem);
        }
    }

    doc.SaveFile(xmlPath.toUtf8().constData());
    _logs->Log(LogLevel::INFO, "[SERVERS] Saved %zu server(s) to servers.xml.", srvData.size());
}

void ServersList::loadServers()
{
    QString xmlPath = GetPath("servers.xml");
    if (xmlPath.isEmpty())
        return;

    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(xmlPath.toUtf8().constData()) != tinyxml2::XML_SUCCESS)
    {
        _logs->Log(LogLevel::WARN, "[SERVERS] Could not open '%s'. Using fallback default server.", xmlPath.toUtf8().constData());
        // Fallback: populate with server from settings or 127.0.0.1:7777
        srvDTA defSrv;
        defSrv.address = (strlen(settings.server.szAddr) > 0) ? settings.server.szAddr : "127.0.0.1";
        defSrv.port = (settings.server.iPort > 0) ? settings.server.iPort : 7777;
        addServer(defSrv);
        return;
    }

    tinyxml2::XMLElement* root = doc.FirstChildElement("servers");
    if (!root)
        return;

    for (tinyxml2::XMLElement* elem = root->FirstChildElement("server"); elem; elem = elem->NextSiblingElement("server"))
    {
        const char* addr = elem->Attribute("address");
        int port = 7777;
        elem->QueryIntAttribute("port", &port);
        if (addr && strlen(addr) > 0)
        {
            srvDTA srv;
            srv.address = addr;
            srv.port = port;
            addServer(srv);
        }
    }

    if (srvData.empty())
    {
        srvDTA defSrv;
        defSrv.address = (strlen(settings.server.szAddr) > 0) ? settings.server.szAddr : "127.0.0.1";
        defSrv.port = (settings.server.iPort > 0) ? settings.server.iPort : 7777;
        addServer(defSrv);
    }
}

void ServersList::queryServerAsync(int serverID, const std::string& address, int port)
{
    QPointer<ServersList> self(this);
    QThreadPool::globalInstance()->start(QRunnable::create([self, serverID, address, port]()
    {
        QueryAPI::SAMPServerInfo serverInfo;
        bool success = false;
        QueryAPI::GetSAMPServerInfo(address.c_str(), static_cast<uint16_t>(port), serverInfo, success);

        QString srvName = success ? QString::fromUtf8(serverInfo.serverName.c_str()) : QString();
        int pingVal = success ? static_cast<int>(serverInfo.ping) : -1;

        if (!self)
            return;

        QMetaObject::invokeMethod(self.data(), [self, serverID, srvName, pingVal, success]()
        {
            if (self)
                self->updateServerRow(serverID, srvName, pingVal, success);
        }, Qt::QueuedConnection);
    }));
}

void ServersList::updateServerRow(int serverID, const QString& name, int ping, bool online)
{
    if (!serverL || srvData.find(serverID) == srvData.end())
        return;

    if (serverID < 0 || serverID >= serverL->rowCount())
        return;

    if (online)
    {
        serverL->setItem(serverID, 1, new QTableWidgetItem(name.isEmpty() ? QString::fromUtf8("SA-MP Server") : name));
        serverL->setItem(serverID, 4, new QTableWidgetItem(QString::number(ping)));
    }
    else
    {
        auto* currentNameItem = serverL->item(serverID, 1);
        if (!currentNameItem || currentNameItem->text() == "Querying...")
            serverL->setItem(serverID, 1, new QTableWidgetItem(QString::fromUtf8("<OFFLINE>")));
        serverL->setItem(serverID, 4, new QTableWidgetItem(QString::fromUtf8("<OFFLINE>")));
    }
}

bool ServersList::addServer(const srvDTA& server)
{
    if (serverL == nullptr)
        return false;

    // Deduplicate: check if ip:port already exists
    for (const auto& [id, dta] : srvData)
    {
        if (dta.size() >= 2 && std::get<std::string>(dta[0]) == server.address && std::get<int>(dta[1]) == server.port)
        {
            return true;
        }
    }

    // Find the lowest available ID
    int serverID = -1;
    for (int i = 0; i < MAX_SERVERS; ++i)
    {
        if (srvData.find(i) == srvData.end())
        {
            serverID = i;
            break;
        }
    }
    if (serverID < 0)
        return false;

    if (serverID >= serverL->rowCount())
        serverL->setRowCount(serverID + 1);

    if (!isTableWidgetRowEmpty(serverL, serverID))
    {
        for (int col = 0; col < serverL->columnCount(); ++col)
            serverL->takeItem(serverID, col);
    }

    serverL->setItem(serverID, 0, new QTableWidgetItem(QString::number(serverID)));
    serverL->setItem(serverID, 1, new QTableWidgetItem(server.name.empty() ? QString::fromUtf8("Querying...") : QString::fromUtf8(server.name.c_str())));
    serverL->setItem(serverID, 2, new QTableWidgetItem(QString::fromUtf8(server.address.c_str())));
    serverL->setItem(serverID, 3, new QTableWidgetItem(QString::number(server.port)));
    serverL->setItem(serverID, 4, new QTableWidgetItem(server.ping >= 0 ? QString::number(server.ping) : QString::fromUtf8("-")));

    srvData[serverID] = std::vector<ListValue>{
        server.address,
        server.port
    };

    _logs->Log(LogLevel::INFO, "[SERVERS] Added server %s:%d (Slot: %d).", server.address.c_str(), server.port, serverID);
    queryServerAsync(serverID, server.address, server.port);
    return true;
}

bool ServersList::removeServer(int serverid)
{
    if (serverL == nullptr)
        return false;
    if (serverid < 0 || serverid >= serverL->rowCount())
        return false;

    if (srvData.find(serverid) == srvData.end())
        return false;

    for (int col = 0; col < serverL->columnCount(); ++col)
        serverL->takeItem(serverid, col);

    srvData.erase(serverid);
    _logs->Log(LogLevel::INFO, "[SERVERS] Removed server at slot %d.", serverid);
    return true;
}

void ServersList::onTick()
{
    tickCounter++;
    // Refresh servers asynchronously every ~5 seconds (7 ticks at 750ms interval)
    if (tickCounter % 7 != 0)
        return;

    if (!this->isVisible())
        return;

    if (srvData.empty())
        return;

    bool expected = false;
    if (!isQuerying.compare_exchange_strong(expected, true))
        return;

    auto serversSnapshot = srvData;
    QPointer<ServersList> self(this);

    QThreadPool::globalInstance()->start(QRunnable::create([self, serversSnapshot]()
    {
        for (const auto& [serverID, serverDTA] : serversSnapshot)
        {
            if (!self)
                break;
            if (serverDTA.size() < 2)
                continue;
            std::string addr = std::get<std::string>(serverDTA[0]);
            int port = std::get<int>(serverDTA[1]);

            QueryAPI::SAMPServerInfo serverInfo;
            bool success = false;
            QueryAPI::GetSAMPServerInfo(addr.c_str(), static_cast<uint16_t>(port), serverInfo, success);

            QString srvName = success ? QString::fromUtf8(serverInfo.serverName.c_str()) : QString();
            int pingVal = success ? static_cast<int>(serverInfo.ping) : -1;

            if (!self)
                break;

            QMetaObject::invokeMethod(self.data(), [self, serverID, srvName, pingVal, success]()
            {
                if (self)
                    self->updateServerRow(serverID, srvName, pingVal, success);
            }, Qt::QueuedConnection);
        }

        if (self)
            self->isQuerying.store(false);
    }));
}
