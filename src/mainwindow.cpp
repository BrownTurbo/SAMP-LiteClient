#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("MainWindow"));
    this->resize(1189, 586);
    this->setFixedSize(1189, 586);
    QWidget *centralwidget = new QWidget(this);

    if (centralwidget->objectName().isEmpty())
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    chatDisplay = new QTextBrowser(centralwidget);
    if (chatDisplay->objectName().isEmpty())
        chatDisplay->setObjectName(QString::fromUtf8("chatDisplay"));
    chatDisplay->setGeometry(QRect(9, 9, 861, 501));
    chatDisplay->setReadOnly(true);
#if QT_CONFIG(whatsthis)
    chatDisplay->setWhatsThis(QString::fromUtf8("Chat Box"));
#endif
    chatDisplay->setReadOnly(true);
    messageInput = new QLineEdit(centralwidget);
    if (messageInput->objectName().isEmpty())
        messageInput->setObjectName(QString::fromUtf8("messageInput"));
    messageInput->setGeometry(QRect(10, 520, 591, 30));
    messageInput->setPlaceholderText(QString::fromUtf8("Type your message here..."));
    messageInput->setEnabled(false);
    sendButton = new QPushButton(centralwidget);
    if (sendButton->objectName().isEmpty())
        sendButton->setObjectName(QString::fromUtf8("sendButton"));
    sendButton->setGeometry(QRect(610, 520, 60, 30));
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
    stateMsg->setGeometry(QRect(10, 551, 861, 31));
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
    pauseButton->setGeometry(QRect(1160, 560, 20, 20));
    pauseButton->setIcon(QIcon(GetPath("resources/img/audio-pause.png")));
    pauseButton->setToolTip(QString::fromUtf8("Pause"));
    pauseButton->setCursor(Qt::PointingHandCursor);
    pauseButton->setEnabled(false);
    stopButton = new QPushButton(centralwidget);
    stopButton->setObjectName(QString::fromUtf8("stopButton"));
    stopButton->setGeometry(QRect(1130, 560, 20, 20));
    stopButton->setIcon(QIcon(GetPath("resources/img/audio-stop.png")));
    stopButton->setToolTip(QString::fromUtf8("Stop"));
    stopButton->setCursor(Qt::PointingHandCursor);
    stopButton->setEnabled(false);
    audioCButton = new QPushButton(centralwidget);
    audioCButton->setObjectName(QString::fromUtf8("audioCButton"));
    audioCButton->setGeometry(QRect(1100, 560, 20, 20));
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

    this->setCentralWidget(centralwidget);

    this->connect(sendButton, &QPushButton::clicked, this, &MainWindow::sendMessage);
    this->connect(messageInput, &QLineEdit::returnPressed, this, &MainWindow::sendMessage);
    this->connect(logsButton, &QPushButton::clicked, this, &MainWindow::showLogsWindow);
    this->connect(toolButton, &QPushButton::clicked, this, &MainWindow::showSettingsWindow);
    this->connect(rootbtn, &QPushButton::clicked, this, &MainWindow::showConnectWindow);
    this->connect(scoreboardbtn, &QPushButton::clicked, this, &MainWindow::showScoreBoardWindow);
    this->connect(playbackbtn, &QPushButton::clicked, this, &MainWindow::showPlaybackWindow);

    // Threading shit!!
    threadManager = new ThreadManager(3);

    // Workers...
    worker = new WorkerClass(this->centralWidget());

    workerThread = new QThread(this);
    connect(workerThread, &QThread::finished, worker, &QObject::deleteLater);
    threadManager->queueWorker(workerThread);
    threadManager->processNextWorker();

    this->connect(pauseButton, &QPushButton::clicked, this, &MainWindow::onAudioPauseClicked);
    this->connect(stopButton, &QPushButton::clicked, this, &MainWindow::onAudioStopClicked);
    this->connect(audioCButton, &QPushButton::clicked, this, &MainWindow::onMuteClicked);

    this->setWindowTitle(QString::fromUtf8("Chat Application"));

    QMetaObject::connectSlotsByName(this);

    Globals::instance().setChatD(QSharedPointer<QTextBrowser>(chatDisplay, [](QTextBrowser * ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));
    Globals::instance().setStateL(QSharedPointer<QLabel>(stateMsg, [](QLabel* ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setMessageInput(QSharedPointer<QLineEdit>(messageInput, [](QLineEdit * ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));
    Globals::instance().setSendButton(QSharedPointer<QPushButton>(sendButton, [](QPushButton * ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setConnectButton(QSharedPointer<QPushButton>(rootbtn, [](QPushButton *ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setPlaybackButton(QSharedPointer<QPushButton>(playbackbtn, [](QPushButton *ptr)
                                                                     {
        if (ptr && !ptr->parent())
            ptr->deleteLater(); }));

    Globals::instance().setCentralWidget(this->centralWidget());

    Globals::instance().setPropertiesTree(QSharedPointer<QStandardItemModel>(treeVmodel, [](QStandardItemModel* ptr) {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setStopAudioButton(QSharedPointer<QPushButton>(stopButton, [](QPushButton *ptr)
    {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setPauseAudioButton(QSharedPointer<QPushButton>(pauseButton, [](QPushButton *ptr)
    {
        if (ptr && !ptr->parent())
            ptr->deleteLater();
    }));

    Globals::instance().setMuteAudioButton(QSharedPointer<QPushButton>(audioCButton, [](QPushButton *ptr)
    {
        if (ptr && !ptr->parent())
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

    // SAMP...
    Nworker = new RakNetWorker();
    threadManager->queueWorker(Nworker);
    threadManager->processNextWorker();
}

MainWindow::~MainWindow() {
    threadManager->stopAllThreads(3000);
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

        Logs LogI;
        LogI.Log(LogLevel::DEBUG, "New Message is sent...");

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

    QDockWidget *dock = new QDockWidget("Logs", this);
    dock->setWidget(logsWindow);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::NoDockWidgetArea);
    dock->resize(logsWindow->sizeHint());

    centerOverParent(dock, this);
    dock->show();
}

void MainWindow::showSettingsWindow()
{
    if (!settingsWindow)
        settingsWindow = new SettingsWindow(this);

    QDockWidget *dock = new QDockWidget("Settings", this);
    dock->setWidget(settingsWindow);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::NoDockWidgetArea);
    dock->resize(settingsWindow->sizeHint());

    centerOverParent(dock, this);
    dock->show();
}

void MainWindow::showConnectWindow()
{
    if(GetGameState() == GAMESTATE_CONNECTED ||
       GetGameState() == GAMESTATE_CONNECTION_REQUESTED ||
       GetGameState() == GAMESTATE_CONNECTING)
    {
        AppendChatBox("Disconnection Requested.");
        AppendLogF("Disconnection Requested.");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        SetGameState(GAMESTATE_DISCONNECTION_REQUESTED);
    }
    else
    {
        if (!connectWindow)
            connectWindow = new ConnectWindow(this);

        QDockWidget *dock = new QDockWidget("Connect", this);
        dock->setWidget(connectWindow);
        dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
        dock->setFloating(true);
        dock->setAllowedAreas(Qt::NoDockWidgetArea);
        dock->resize(connectWindow->sizeHint());

        centerOverParent(dock, this);
        dock->show();
    }
}

void MainWindow::showScoreBoardWindow()
{
    if (!scoreboardWindow)
        scoreboardWindow = new Scoreboard(this);

    QDockWidget *dock = new QDockWidget("Scoreboard", this);
    dock->setWidget(scoreboardWindow);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::NoDockWidgetArea);
    dock->resize(scoreboardWindow->sizeHint());

    centerOverParent(dock, this);
    dock->show();
}

void MainWindow::showPlaybackWindow()
{
    if (!playbackWindow)
        playbackWindow = new PlaybackWindow(this);

    QDockWidget *dock = new QDockWidget("Playback", this);
    dock->setWidget(playbackWindow);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    dock->setFloating(true);
    dock->setAllowedAreas(Qt::NoDockWidgetArea);
    dock->resize(playbackWindow->sizeHint());

    centerOverParent(dock, this);
    dock->show();
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
