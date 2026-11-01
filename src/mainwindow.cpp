#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("MainWindow"));
    this->resize(1189, 600);
    this->setFixedSize(1189, 600);
    QWidget *centralwidget = new QWidget(this);

    if (centralwidget->objectName().isEmpty())
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    chatDisplay = new QTextBrowser(centralwidget);
    if (chatDisplay->objectName().isEmpty())
        chatDisplay->setObjectName(QString::fromUtf8("chatDisplay"));
    chatDisplay->setGeometry(QRect(9, 9, 861, 513));
    chatDisplay->setReadOnly(true);
#if QT_CONFIG(whatsthis)
    chatDisplay->setWhatsThis(QString::fromUtf8("Chat Box"));
#endif
    chatDisplay->setReadOnly(true);
    messageInput = new QLineEdit(centralwidget);
    if (messageInput->objectName().isEmpty())
        messageInput->setObjectName(QString::fromUtf8("messageInput"));
    messageInput->setGeometry(QRect(10, 533, 591, 30));
    messageInput->setPlaceholderText(QString::fromUtf8("Type your message here..."));
    messageInput->setEnabled(false);
    sendButton = new QPushButton(centralwidget);
    if (sendButton->objectName().isEmpty())
        sendButton->setObjectName(QString::fromUtf8("sendButton"));
    sendButton->setGeometry(QRect(610, 533, 60, 30));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(sendButton->sizePolicy().hasHeightForWidth());
    sendButton->setSizePolicy(sizePolicy);
    sendButton->setEnabled(true);
    sendButton->setIcon(QIcon(GetPath("resources/img/send.png")));
    sendButton->setIconSize(QSize(17, 17));
    sendButton->setText(QString::fromUtf8("Send"));
    sendButton->setEnabled(false);
    stateProgressBar = new QProgressBar(centralwidget);
    if (stateProgressBar->objectName().isEmpty())
        stateProgressBar->setObjectName(QString::fromUtf8("stateProgressBar"));
    stateProgressBar->setEnabled(false);
    stateProgressBar->setGeometry(QRect(890, 460, 281, 21));
    sizePolicy.setHeightForWidth(stateProgressBar->sizePolicy().hasHeightForWidth());
    stateProgressBar->setSizePolicy(sizePolicy);
    stateProgressBar->setValue(24);
    /*line = new QFrame(centralwidget);
    if (line->objectName().isEmpty())
        line->setObjectName(QString::fromUtf8("line"));
    line->setGeometry(QRect(9, 519, 16, 16));
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);*/
    toolButton = new QPushButton(centralwidget);
    if (toolButton->objectName().isEmpty())
        toolButton->setObjectName(QString::fromUtf8("toolButton"));
    toolButton->setGeometry(QRect(1100, 420, 77, 29));
    toolButton->setIcon(QIcon(GetPath("resources/img/settings.png")));
    toolButton->setIconSize(QSize(17, 17));
    toolButton->setText(QString::fromUtf8("Settings"));
    toolButton->setCursor(Qt::PointingHandCursor);
    rootbtn = new QPushButton(centralwidget);
    if (rootbtn->objectName().isEmpty())
        rootbtn->setObjectName(QString::fromUtf8("rootbtn"));
    rootbtn->setEnabled(true);
    rootbtn->setGeometry(QRect(890, 490, 79, 29));
    rootbtn->setIcon(QIcon(GetPath("resources/img/game.png")));
    rootbtn->setIconSize(QSize(17, 17));
    rootbtn->setText(QString::fromUtf8("Connect"));
    rootbtn->setCursor(Qt::PointingHandCursor);
    treeView = new QTreeView(centralwidget);
    if (treeView->objectName().isEmpty())
        treeView->setObjectName(QString::fromUtf8("treeView"));
    treeView->setGeometry(QRect(885, 9, 291, 401));
    treeVmodel = new QStandardItemModel(this);
    treeVmodel->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
    treeView->setModel(treeVmodel);
    treeView->header()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    treeView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    treeView->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    stateMsg = new QLabel(centralwidget);
    if (stateMsg->objectName().isEmpty())
        stateMsg->setObjectName(QString::fromUtf8("stateMsg"));
    stateMsg->setEnabled(true);
    stateMsg->setGeometry(QRect(10, 566, 861, 31));
    stateMsg->setText(QString::fromUtf8("<p><span style=\" font-size:11pt;\">State</span>:<span style=\" font-weight:600;\"> ........................................................................................................................................................................................</span></p>"));
    logsButton = new QPushButton(centralwidget);
    if (logsButton->objectName().isEmpty())
        logsButton->setObjectName(QString::fromUtf8("logsButton"));
    logsButton->setGeometry(QRect(1036, 420, 63, 30));
    logsButton->setEnabled(true);
    logsButton->setIcon(QIcon(GetPath("resources/img/logs.png")));
    logsButton->setIconSize(QSize(22, 22));
    logsButton->setText(QString::fromUtf8("Logs"));
    logsButton->setCursor(Qt::PointingHandCursor);
    scoreboardbtn = new QPushButton(centralwidget);
    if (scoreboardbtn->objectName().isEmpty())
        scoreboardbtn->setObjectName(QString::fromUtf8("scoreboardbtn"));
    scoreboardbtn->setGeometry(QRect(936, 420, 98, 30));
    sizePolicy.setHeightForWidth(scoreboardbtn->sizePolicy().hasHeightForWidth());
    scoreboardbtn->setSizePolicy(sizePolicy);
    scoreboardbtn->setAutoDefault(false);
    scoreboardbtn->setFlat(false);
    scoreboardbtn->setIcon(QIcon(GetPath("resources/img/scoreboard.png")));
    scoreboardbtn->setIconSize(QSize(22, 22));
    scoreboardbtn->setText(QString::fromUtf8("Scoreboard"));
    scoreboardbtn->setCursor(Qt::PointingHandCursor);
    scoreboardbtn->setEnabled(false);
    pauseButton = new QPushButton(centralwidget);
    pauseButton->setObjectName(QString::fromUtf8("pauseButton"));
    pauseButton->setGeometry(QRect(1160, 575, 20, 20));
    pauseButton->setIcon(QIcon(GetPath("resources/img/audio-pause.png")));
    pauseButton->setToolTip(QString::fromUtf8("Pause"));
    pauseButton->setCursor(Qt::PointingHandCursor);
    pauseButton->setEnabled(false);
    stopButton = new QPushButton(centralwidget);
    stopButton->setObjectName(QString::fromUtf8("stopButton"));
    stopButton->setGeometry(QRect(1130, 575, 20, 20));
    stopButton->setIcon(QIcon(GetPath("resources/img/audio-stop.png")));
    stopButton->setToolTip(QString::fromUtf8("Stop"));
    stopButton->setCursor(Qt::PointingHandCursor);
    stopButton->setEnabled(false);
    audioCButton = new QPushButton(centralwidget);
    audioCButton->setObjectName(QString::fromUtf8("audioCButton"));
    audioCButton->setGeometry(QRect(1100, 575, 20, 20));
    audioCButton->setToolTip(QString::fromUtf8("Mute"));
    audioCButton->setIcon(QIcon(GetPath("resources/img/volume-mute.png")));
    //audioCButton->setIconSize(QSize(65, 65));
    audioCButton->setCursor(Qt::PointingHandCursor);
    audioCButton->setEnabled(false);
    actionsButton = new QPushButton(centralwidget);
    actionsButton->setObjectName(QString::fromUtf8("actionsButton"));
    actionsButton->setGeometry(QRect(1090, 490, 80, 30));
    actionsButton->setText(QString::fromUtf8("Actions"));
    actionsButton->setToolTip(QString::fromUtf8("Actions"));
    actionsButton->setIcon(QIcon(GetPath("resources/img/run.png")));
    actionsButton->setIconSize(QSize(20, 20));
    actionsButton->setCursor(Qt::PointingHandCursor);
    playbackbtn = new QPushButton(centralwidget);
    if (playbackbtn->objectName().isEmpty())
        playbackbtn->setObjectName(QString::fromUtf8("playbackbtn"));
    playbackbtn->setGeometry(QRect(1000, 490, 85, 30));
    sizePolicy.setHeightForWidth(playbackbtn->sizePolicy().hasHeightForWidth());
    playbackbtn->setSizePolicy(sizePolicy);
    playbackbtn->setAutoDefault(false);
    playbackbtn->setFlat(false);
    playbackbtn->setIcon(QIcon(GetPath("resources/img/playback.png")));
    playbackbtn->setIconSize(QSize(22, 22));
    playbackbtn->setText(QString::fromUtf8("Playback"));
    playbackbtn->setCursor(Qt::PointingHandCursor);
    playbackbtn->setEnabled(false);
    serversList = new QPushButton(centralwidget);
    if (serversList->objectName().isEmpty())
        serversList->setObjectName(QString::fromUtf8("serversList"));
    serversList->setGeometry(QRect(1090, 530, 80, 30));
    sizePolicy.setHeightForWidth(serversList->sizePolicy().hasHeightForWidth());
    serversList->setSizePolicy(sizePolicy);
    serversList->setAutoDefault(false);
    serversList->setFlat(false);
    serversList->setIcon(QIcon(GetPath("resources/img/playback.png")));
    serversList->setIconSize(QSize(22, 22));
    serversList->setText(QString::fromUtf8("Servers"));
    serversList->setCursor(Qt::PointingHandCursor);
    serversList->setObjectName(QString::fromUtf8("serversList"));

    this->setCentralWidget(centralwidget);

    this->connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    this->connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    this->connect(logsButton, &QPushButton::clicked, this, &MainWindow::showLogsWindow);
    this->connect(toolButton, &QPushButton::clicked, this, &MainWindow::showSettingsWindow);
    this->connect(rootbtn, &QPushButton::clicked, this, &MainWindow::showConnectWindow);
    this->connect(scoreboardbtn, &QPushButton::clicked, this, &MainWindow::showScoreBoardWindow);
    this->connect(playbackbtn, &QPushButton::clicked, this, &MainWindow::showPlaybackWindow);
    this->connect(serversList, &QPushButton::clicked, this, &MainWindow::showServerLWindow);

    // Threading shit!!
    threadManager = new ThreadManager(3);

    // Workers...
    worker = new WorkerClass(this->centralWidget());

    workerThread = new QThread(this);
    this->connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    threadManager->queueWorker(workerThread);
    threadManager->processNextWorker();

    // Create serverlstWindow BEFORE the intervalWorker connect so the pointer is valid.
    if (!serverlstWindow)
        serverlstWindow = new ServersList(this);

    // ...
    auto* intervalWorker = new IntervalWorker(750, [](){});

    this->connect(intervalWorker, &IntervalWorker::tick, serverlstWindow, &ServersList::onTick, Qt::QueuedConnection);
    threadManager->queueWorker(intervalWorker);
    threadManager->processNextWorker();

    this->connect(pauseButton, &QPushButton::clicked, this, &MainWindow::onAudioPauseClicked);
    this->connect(stopButton, &QPushButton::clicked, this, &MainWindow::onAudioStopClicked);
    this->connect(audioCButton, &QPushButton::clicked, this, &MainWindow::onMuteClicked);

    this->setWindowTitle(QString::fromUtf8("Chat Application"));

    QMetaObject::connectSlotsByName(this);

    QPointer<QTextBrowser> guardedChat(chatDisplay);
    Globals::instance().setChatD(QSharedPointer<QTextBrowser>(chatDisplay, [guardedChat](QTextBrowser * ptr) {
        if (guardedChat && ptr && !guardedChat->parent())
            ptr->deleteLater();
    }));

    QPointer<QLabel> guardedState(stateMsg);
    Globals::instance().setStateL(QSharedPointer<QLabel>(stateMsg, [guardedState](QLabel* ptr) {
        if (guardedState && ptr && !guardedState->parent())
            ptr->deleteLater();
    }));

    QPointer<QLineEdit> guardedInput(messageInput);
    Globals::instance().setMessageInput(QSharedPointer<QLineEdit>(messageInput, [guardedInput](QLineEdit * ptr) {
        if (guardedInput && ptr && !guardedInput->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedSend(sendButton);
    Globals::instance().setSendButton(QSharedPointer<QPushButton>(sendButton, [guardedSend](QPushButton * ptr) {
        if (guardedSend && ptr && !guardedSend->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedConnect(rootbtn);
    Globals::instance().setConnectButton(QSharedPointer<QPushButton>(rootbtn, [guardedConnect](QPushButton *ptr) {
        if (guardedConnect && ptr && !guardedConnect->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedPlayback(playbackbtn);
    Globals::instance().setPlaybackButton(QSharedPointer<QPushButton>(playbackbtn, [guardedPlayback](QPushButton *ptr) {
        if (guardedPlayback && ptr && !guardedPlayback->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setCentralWidget(this->centralWidget());

    QPointer<QStandardItemModel> guardedTree(treeVmodel);
    Globals::instance().setPropertiesTree(QSharedPointer<QStandardItemModel>(treeVmodel, [guardedTree](QStandardItemModel* ptr) {
        if (guardedTree && ptr && !guardedTree->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedStop(stopButton);
    Globals::instance().setStopAudioButton(QSharedPointer<QPushButton>(stopButton, [guardedStop](QPushButton *ptr) {
        if (guardedStop && ptr && !guardedStop->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedPause(pauseButton);
    Globals::instance().setPauseAudioButton(QSharedPointer<QPushButton>(pauseButton, [guardedPause](QPushButton *ptr) {
        if (guardedPause && ptr && !guardedPause->parent())
            ptr->deleteLater();
    }));

    QPointer<QPushButton> guardedMute(audioCButton);
    Globals::instance().setMuteAudioButton(QSharedPointer<QPushButton>(audioCButton, [guardedMute](QPushButton *ptr) {
        if (guardedMute && ptr && !guardedMute->parent())
            ptr->deleteLater();
    }));

    // Lazy weird fix
    if (!logsWindow)
        logsWindow = new LogsWindow(this);
    centerOverParent(logsWindow, this);
    logsWindow->show();
    logsWindow->hide();

    if (!settingsWindow)
        settingsWindow = new SettingsWindow(this);
    centerOverParent(settingsWindow, this);
    settingsWindow->show();
    settingsWindow->hide();

    if (!connectWindow)
        connectWindow = new ConnectWindow(this);
    centerOverParent(connectWindow, this);
    connectWindow->show();
    connectWindow->hide();

    if (!playbackWindow)
        playbackWindow = new PlaybackWindow(this);
    centerOverParent(playbackWindow, this);
    playbackWindow->show();
    playbackWindow->hide();

    if (!scoreboardWindow)
        scoreboardWindow = new Scoreboard(this);
    centerOverParent(scoreboardWindow, this);
    scoreboardWindow->show();
    scoreboardWindow->hide();

    if (!serverlstWindow)
        serverlstWindow = new ServersList(this);
    centerOverParent(serverlstWindow, this);
    serverlstWindow->show();
    serverlstWindow->hide();

    // SAMP...
    Nworker = new RakNetWorker();
    threadManager->queueWorker(Nworker);
    threadManager->processNextWorker();
}

MainWindow::~MainWindow() {
    threadManager->stopAllThreads(3000);

    Globals::instance().setChatD(nullptr);
    Globals::instance().setStateL(nullptr);
    Globals::instance().setMessageInput(nullptr);
    Globals::instance().setSendButton(nullptr);
    Globals::instance().setConnectButton(nullptr);
    Globals::instance().setPlaybackButton(nullptr);
    Globals::instance().setCentralWidget(nullptr);
    Globals::instance().setPropertiesTree(nullptr);
    Globals::instance().setStopAudioButton(nullptr);
    Globals::instance().setPauseAudioButton(nullptr);
    Globals::instance().setMuteAudioButton(nullptr);
    Globals::instance().setLogsD(nullptr);
    Globals::instance().setLogsW(nullptr);
}

void MainWindow::sendMessage() {
    if (GetGameState() != GAMESTATE_CONNECTED)
        return;
    QString message = messageInput->text();
    if (!message.isEmpty()) {
        QString output = removeMarkupTags(message);
        QByteArray msgBYTEArr = output.toUtf8();
        char *_msg = msgBYTEArr.data();
        AppendChatBox("<b>You:</b> %s", output);
        messageInput->clear();

        _logs->Log(LogLevel::INFO, "Chat Input: %s", output.toUtf8().constData());

        if (_msg[0] == '/')
            sendServerCommand(_msg);
        else if (_msg[0] == '!')
            RunCommand(_msg);
        else
            sendChat(_msg);
    } else
        emit worker->MessageBox(QString::fromUtf8("Warning"), QString::fromUtf8("Message cannot be empty!"), QMessageBox::Ok, QMessageBox::Warning);
}

void MainWindow::showLogsWindow()
{
    if (!logsWindow)
        logsWindow = new LogsWindow(this);

    if (!logsDock)
    {
        logsDock = new QDockWidget("Logs", this);
        logsDock->setWidget(logsWindow);
        logsDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        logsDock->setFloating(true);
        logsDock->setAllowedAreas(Qt::NoDockWidgetArea);
        logsDock->resize(logsWindow->sizeHint());
        centerOverParent(logsDock, this);
    }

    logsDock->show();
    logsDock->raise();
    logsDock->activateWindow();
}

void MainWindow::showSettingsWindow()
{
    if (!settingsWindow)
        settingsWindow = new SettingsWindow(this);

    if (!settingsDock)
    {
        settingsDock = new QDockWidget("Settings", this);
        settingsDock->setWidget(settingsWindow);
        settingsDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        settingsDock->setFloating(true);
        settingsDock->setAllowedAreas(Qt::NoDockWidgetArea);
        settingsDock->resize(settingsWindow->sizeHint());
        centerOverParent(settingsDock, this);
    }

    settingsDock->show();
    settingsDock->raise();
    settingsDock->activateWindow();
}

void MainWindow::showConnectWindow()
{
    if(GetGameState() == GAMESTATE_CONNECTED ||
       GetGameState() == GAMESTATE_CONNECTION_REQUESTED ||
       GetGameState() == GAMESTATE_CONNECTING)
    {
        AppendChatBox("Disconnection Requested.");
        AppendLogF("Disconnection Requested.");
        _logs->Log(LogLevel::INFO, "[MAIN] Disconnection Requested by user.");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
    }
    else
    {
        if (!connectWindow)
            connectWindow = new ConnectWindow(this);

        connectWindow->updateServerInfo();

        if (!connectDock)
        {
            connectDock = new QDockWidget("Connect", this);
            connectDock->setWidget(connectWindow);
            connectDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
            connectDock->setFloating(true);
            connectDock->setAllowedAreas(Qt::NoDockWidgetArea);
            connectDock->resize(connectWindow->sizeHint());
            centerOverParent(connectDock, this);
        }

        connectDock->show();
        connectDock->raise();
        connectDock->activateWindow();
    }
}

void MainWindow::showScoreBoardWindow()
{
    if (!scoreboardWindow)
        scoreboardWindow = new Scoreboard(this);

    if (!scoreboardDock)
    {
        scoreboardDock = new QDockWidget("Scoreboard", this);
        scoreboardDock->setWidget(scoreboardWindow);
        scoreboardDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        scoreboardDock->setFloating(true);
        scoreboardDock->setAllowedAreas(Qt::NoDockWidgetArea);
        scoreboardDock->resize(scoreboardWindow->sizeHint());
        centerOverParent(scoreboardDock, this);
    }

    scoreboardDock->show();
    scoreboardDock->raise();
    scoreboardDock->activateWindow();
}

void MainWindow::showPlaybackWindow()
{
    if (!playbackWindow)
        playbackWindow = new PlaybackWindow(this);

    if (!playbackDock)
    {
        playbackDock = new QDockWidget("Playback", this);
        playbackDock->setWidget(playbackWindow);
        playbackDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        playbackDock->setFloating(true);
        playbackDock->setAllowedAreas(Qt::NoDockWidgetArea);
        playbackDock->resize(playbackWindow->sizeHint());
        centerOverParent(playbackDock, this);
    }

    playbackDock->show();
    playbackDock->raise();
    playbackDock->activateWindow();
}

void MainWindow::showServerLWindow()
{
    if (!serverlstWindow)
        serverlstWindow = new ServersList(this);

    if (!serverlstDock)
    {
        serverlstDock = new QDockWidget("Servers List", this);
        serverlstDock->setWidget(serverlstWindow);
        serverlstDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        serverlstDock->setFloating(true);
        serverlstDock->setAllowedAreas(Qt::NoDockWidgetArea);
        serverlstDock->resize(serverlstWindow->sizeHint());
        centerOverParent(serverlstDock, this);
    }

    serverlstDock->show();
    serverlstDock->raise();
    serverlstDock->activateWindow();
}

void MainWindow::onAudioPauseClicked()
{
    AudioStream audioStream;
    audioStream.PauseAudioStream();
}

void MainWindow::onAudioStopClicked()
{
    AudioStream audioStream;
    audioStream.StopAudioStream();
}

void MainWindow::onMuteClicked()
{
    QIcon currIcon = audioCButton->icon();
    QList<QSize> sizes = currIcon.availableSizes();
    QSize iconSize;
    AudioStream audioStream;
    if (!sizes.isEmpty())
        iconSize = sizes.first();

    if (currIcon.pixmap(iconSize.width(), iconSize.height()).toImage() == QPixmap(GetPath("resources/img/volume-mute.png")).toImage())
    {
        audioCButton->setIcon(QIcon(GetPath("resources/img/volume.png")));
        audioCButton->setToolTip("Mute");
        if (lastAudioLVL == 0.0)
            lastAudioLVL = 0.5;
        audioStream.SetAudioVolume(lastAudioLVL);
    }
    else
    {
        audioCButton->setIcon(QIcon(GetPath("resources/img/volume-mute.png")));
        audioCButton->setToolTip("UnMute");
        lastAudioLVL = audioStream.GetAudioVolume();
        audioStream.SetAudioVolume(0.0);
    }
}
