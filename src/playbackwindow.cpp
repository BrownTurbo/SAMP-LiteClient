#include "../include/playbackwindow.h"

PlaybackWindow::PlaybackWindow(QWidget *parent) : QGroupBox(parent)
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("playbackWindow"));
    this->resize(950, 600);
    this->setFixedSize(950, 600);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    this->setMaximumSize(QSize(950, 600));
    this->setWindowTitle(QCoreApplication::translate("playbackWindow", "Playback", nullptr));
    tableWidget = new QTableWidget(this);
    if (tableWidget->objectName().isEmpty())
        tableWidget->setObjectName(QString::fromUtf8("PlaybackWidget"));
    tableWidget->setGeometry(QRect(5, 10, 941, 511));
    tableWidget->setFrameShape(QFrame::StyledPanel);
    tableWidget->setFrameShadow(QFrame::Sunken);
    tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    tableWidget->setAutoScroll(false);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Title", "Type"});
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->verticalHeader()->setVisible(false);
    playbtn = new QPushButton(this);
    if (playbtn->objectName().isEmpty())
        playbtn->setObjectName(QString::fromUtf8("playbtn"));
    playbtn->setGeometry(QRect(870, 560, 71, 31));
    playbtn->setCursor(QCursor(Qt::PointingHandCursor));
    playbtn->setText(QCoreApplication::translate("playbackWindow", "Play", nullptr));
    playbtn->setIcon(QIcon(GetPath("resources/img/run.png")));
    playbtn->setIconSize(QSize(20, 20));
    pausebtn = new QPushButton(this);
    if (pausebtn->objectName().isEmpty())
        pausebtn->setObjectName(QString::fromUtf8("pausebtn"));
    pausebtn->setGeometry(QRect(790, 560, 71, 31));
    pausebtn->setCursor(QCursor(Qt::PointingHandCursor));
    pausebtn->setEnabled(false);
    pausebtn->setText(QCoreApplication::translate("playbackWindow", "Pause", nullptr));
    pausebtn->setIcon(QIcon(GetPath("resources/img/audio-pause.png")));
    recordbtn = new QPushButton(this);
    if (recordbtn->objectName().isEmpty())
        recordbtn->setObjectName(QString::fromUtf8("recordbtn"));
    recordbtn->setGeometry(QRect(710, 560, 71, 31));
    recordbtn->setCursor(QCursor(Qt::PointingHandCursor));
    recordbtn->setText(QCoreApplication::translate("playbackWindow", "Record", nullptr));
    closebtn = new QPushButton(this);
    if (closebtn->objectName().isEmpty())
        closebtn->setObjectName(QString::fromUtf8("closebtn"));
    closebtn->setGeometry(QRect(10, 560, 71, 31));
    closebtn->setCursor(QCursor(Qt::PointingHandCursor));
    closebtn->setText(QCoreApplication::translate("playbackWindow", "Close", nullptr));
    statuslabel = new QLabel(this);
    if (statuslabel->objectName().isEmpty())
        statuslabel->setObjectName(QString::fromUtf8("statuslabel"));
    statuslabel->setGeometry(QRect(10, 530, 931, 23));
    // statuslabel->setCursor(QCursor(Qt::WaitCursor));
    statuslabel->setFrameShape(QFrame::StyledPanel);
    statuslabel->setFrameShadow(QFrame::Raised);
    statuslabel->setMidLineWidth(0);
    statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Idle.", nullptr));
    recordType = new QComboBox(this);
    recordType->addItem(QString());
    recordType->addItem(QString());
    if (recordType->objectName().isEmpty())
        recordType->setObjectName(QString::fromUtf8("recordType"));
    recordType->setGeometry(QRect(150, 560, 100, 31));
    recordType->setItemText(0, QCoreApplication::translate("playbackWindow", "OnFoot", nullptr));
    recordType->setItemText(1, QCoreApplication::translate("playbackWindow", "OnVehicle", nullptr));
    QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy1.setHorizontalStretch(7);
    sizePolicy1.setVerticalStretch(0);
    sizePolicy1.setHeightForWidth(recordType->sizePolicy().hasHeightForWidth());
    recordType->setSizePolicy(sizePolicy1);
    recordType->setMaximumSize(QSize(83, 30));
    recordType->setMaxVisibleItems(2);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableWidget);
    buttonLayout->addWidget(recordType);
    buttonLayout->addWidget(playbtn);
    buttonLayout->addWidget(pausebtn);
    buttonLayout->addWidget(recordbtn);
    buttonLayout->addWidget(closebtn);
    mainLayout->addWidget(statuslabel);
    mainLayout->addLayout(buttonLayout);
    this->setLayout(mainLayout);

    this->connect(playbtn, &QPushButton::clicked, this, &PlaybackWindow::onPlayPressed);
    this->connect(pausebtn, &QPushButton::clicked, this, &PlaybackWindow::onPausePressed);
    this->connect(recordbtn, &QPushButton::clicked, this, &PlaybackWindow::onRecordPressed);
    this->connect(closebtn, &QPushButton::clicked, this, &PlaybackWindow::onClosePressed);

    QMetaObject::connectSlotsByName(this);

    UpdStatusTimer = new QTimer(this);
    this->connect(UpdStatusTimer, &QTimer::timeout, this, &PlaybackWindow::updateStatusElem);

    unsigned short recordsCount = 0;
    unsigned short linesCount = 0;
    QDir records(Playback::szPlaybackDirectory, "*.rec", QDir::IgnoreCase, QDir::Files | QDir::Readable);
    for (auto const & fileName : records.entryInfoList())
    {
        recordsCount += 1;
        if(recordsCount > MAX_RECORDS)
            break;
        QFile recF{fileName.absoluteFilePath()};
        recordsData[(recordsCount - 1)].recF = recF.fileName();
        if (recF.open(QFile::ReadOnly | QFile::Text))
        {
            while (!recF.atEnd())
            {
                linesCount += 1;
                QByteArray line = recF.readLine();
                char const* data = line.constData();
                unsigned short len = line.size();
                unsigned short idx = 0;
                if (idx < len) {
                    if (strncmp(data + idx, "### ", 4) == 0) {
                        idx += 4;
                        QString __Data = QString::fromStdString(std::string(data + idx, (size_t)len - idx + 1));
                        QStringList vars = __Data.split(';');
                        for(int varIDX = 0; varIDX < vars.size(); varIDX++)
                        {
                            QStringList var = vars.at(varIDX).split('=');
                            QString vName = var.at(0);
                            QString vValue = var.at(1);
                            if (strcmp(vName.toUtf8().data(), "title"))
                            {
                                recordsData[(recordsCount - 1)].recTitle = vValue.toUtf8().data();
                            }
                        }
                    }
                    else if (strncmp(data + idx, "# ", 2) == 0) {
                        idx += 2;
                        QString __Data = QString::fromStdString(std::string(data + idx, (size_t)len - idx + 1));
                        QStringList vars = __Data.split(';');
                        for (int varIDX = 0; varIDX < vars.size(); varIDX++)
                        {
                            QStringList var = vars.at(varIDX).split('=');
                            QString vName = var.at(0);
                            QString vValue = var.at(1);
                            if (strcmp(vName.toUtf8().data(), "pos"))
                            {
                                QStringList coords = vValue.split(",");
                                float X = coords.at(0).toFloat();
                                float Y = coords.at(1).toFloat();
                                float Z = coords.at(2).toFloat();
                                recordsData[(recordsCount - 1)].recPos[(linesCount - 1)].X = X;
                                recordsData[(recordsCount - 1)].recPos[(linesCount - 1)].Y = Y;
                                recordsData[(recordsCount - 1)].recPos[(linesCount - 1)].Z = Z;
                            }
                            else if (strcmp(vName.toUtf8().data(), "type"))
                            {
                                unsigned short recordType = vValue.toUShort();
                                switch (recordType)
                                {
                                    case 0:
                                    {
                                        recordsData[(recordsCount - 1)].recType = Playback::PLAYBACK_TYPE_NONE;
                                        break;
                                    }
                                    case 1:
                                    {
                                        recordsData[(recordsCount - 1)].recType = Playback::PLAYBACK_TYPE_ONFOOT;
                                        break;
                                    }
                                    case 2:
                                    {
                                        recordsData[(recordsCount - 1)].recType = Playback::PLAYBACK_TYPE_DRIVER;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            tableWidget->setItem((recordsCount - 1), 0, new QTableWidgetItem(recordsData[(recordsCount - 1)].recTitle));
            tableWidget->setItem((recordsCount - 1), 1, new QTableWidgetItem(QString::fromUtf8(recordsData[(recordsCount - 1)].recType == Playback::PLAYBACK_TYPE_NONE ? "??" : (recordsData[(recordsCount - 1)].recType == Playback::PLAYBACK_TYPE_DRIVER ? "Driver" : "On foot"))));
        }
        recF.close();
    }
}
PlaybackWindow::~PlaybackWindow() {}

void PlaybackWindow::onPlayPressed()
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    if (Playback::bPlaying)
    {
        if (Playback::Unload())
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Idle.", nullptr));
        else
            emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8("You haven't load any playback"), QMessageBox::Ok, QMessageBox::Information);
    }
    else
    {
        Playback::bPlaying = true;
        Playback::StartFirstEver();
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Loading playback...", nullptr));
    }
}

void PlaybackWindow::updateStatusElem()
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    char playbackP[128 + 32];
    if (playerPool[PBRecorder::iTargetPlayer].bIsConnected)
    {
        if (playerPool[PBRecorder::iTargetPlayer].bIsStreamedIn)
        {
            sprintf(playbackP, "Currently: Recording Player %s(ID:%d)'s sync data (%d / %d)...", playerPool[PBRecorder::iTargetPlayer].szPlayerName, PBRecorder::iTargetPlayer, Playback::GetCurrentIndex(), Playback::GetTotalPacket());
            statuslabel->setText(QCoreApplication::translate("playbackWindow", playbackP, nullptr));
        }
        else
        {
            sprintf(playbackP, "The player %s is no longer streamed in!<br />Stopping Recorder...", playerPool[PBRecorder::iTargetPlayer].szPlayerName);
            worker.MessageBox(QString::fromUtf8("Info"), QString::fromUtf8(playbackP), QMessageBox::Ok, QMessageBox::Information);
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Recorded playback successfully.", nullptr));
        }
    }
    else
    {
        sprintf(playbackP, "The player %s is no longer connected!<br />Stopping Recorder...", playerPool[PBRecorder::iTargetPlayer].szPlayerName);
        worker.MessageBox(QString::fromUtf8("Info"), QString::fromUtf8(playbackP), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Recorded playback successfully.", nullptr));
        SavePlayback(RECfileN);
    }
}

void PlaybackWindow::onPausePressed()
{
    TogglePlayback();
}

void PlaybackWindow::onRecordPressed()
{
    int iRecordType = Playback::PLAYBACK_TYPE_NONE;
    switch (recordType->currentIndex())
    {
        case 0:
        {
            iRecordType = Playback::PLAYBACK_TYPE_ONFOOT;
            break;
        }
        case 1:
        {
            iRecordType = Playback::PLAYBACK_TYPE_DRIVER;
            break;
        }
        default:
            iRecordType = Playback::PLAYBACK_TYPE_ONFOOT;
    }
    PlaybackWindow::StartRecording(iRecordType);
}

void PlaybackWindow::onClosePressed()
{
    this->close();
}

void PlaybackWindow::LoadPlayback(bool autoPlay)
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    char errmsg[256];
    char szPBMessage[250];
    if (Playback::Load(szInput, errmsg))
    {
        strcpy(PlaybackFile, szInput);
        if (autoPlay)
        {
            Playback::StartFirstEver();
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Loaded & Started Playback successfully.", nullptr));
            sprintf(szPBMessage, "Playback \"%s\" loaded and started Successfully", szInput);
            emit worker.MessageBox(QString::fromUtf8("INFO"), QString::fromUtf8(szPBMessage), QMessageBox::Ok, QMessageBox::Information);
        }
        else
        {
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Loaded Playback successfully.", nullptr));
            sprintf(szPBMessage, "Playback \"%s\" load successful", szInput);
            emit worker.MessageBox(QString::fromUtf8("INFO"), QString::fromUtf8(szPBMessage), QMessageBox::Ok, QMessageBox::Information);
        }
    }
    else
    {
        emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8(errmsg), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Failed to Load playback.", nullptr));
    }
}

void PlaybackWindow::SavePlayback(char szFileName[128])
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    char szPBMessage[128];
    char errmsg[256];
    if (PBRecorder::Finalize(szFileName, errmsg))
    {
        sprintf(szPBMessage, "Playback \"%s\" saved %d packets Successfully", szFileName, PBRecorder::iPacketRecorded);
        emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8(szPBMessage), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Saved Playback successfully.", nullptr));
    }
    else
    {
        emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8(errmsg), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Failed to Save playback.", nullptr));
    }
}

void PlaybackWindow::StartRecording(int iRecordType)
{
    WorkerClass worker(Globals::instance().getCentralWidget());
    worker.moveToThread(QApplication::instance()->thread());
    if (iRecordType == Playback::PLAYBACK_TYPE_NONE)
    {
        emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8("Something went wrong"), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Failed to Save playback.", nullptr));
    }
    else if (!playerPool[PBRecorder::iTargetPlayer].bIsConnected)
    {
        emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8("Player is not connected"), QMessageBox::Ok, QMessageBox::Information);
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Idle.", nullptr));
    }
    else
    {
        if (!playerPool[PBRecorder::iTargetPlayer].bIsStreamedIn)
        {
            emit worker.MessageBox(QString::fromUtf8("ERROR"), QString::fromUtf8("Player is not streamed in"), QMessageBox::Ok, QMessageBox::Information);
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Idle.", nullptr));
        }
        else
        {
            struct tm * st;
            time_t curr_time = time(NULL);
            st = localtime(&curr_time);
            char timeStr[75];
            snprintf(timeStr, sizeof(timeStr), "%s > %d-%d-%d %d:%d:%d", playerPool[PBRecorder::iTargetPlayer].szPlayerName, st->tm_year + 1900, st->tm_mon + 1, st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);
            PBRecorder::StartRecording(iRecordType, timeStr);
            UpdStatusTimer->start(750);
            emit worker.MessageBox(QString::fromUtf8("Info"), QString::fromUtf8("Recording has been started"), QMessageBox::Ok, QMessageBox::Information);
            statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Recording playback.", nullptr));
        }
    }
}

void PlaybackWindow::TogglePlayback()
{
    if(Playback::bPlaying)
    {
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Paused.", nullptr));
        Playback::bPlaying = false;
    }
    else
    {
        statuslabel->setText(QCoreApplication::translate("playbackWindow", "Currently: Loading playback...", nullptr));
        Playback::bPlaying = true;
    }
    if (Playback::GetCurrentIndex() == 0)
        Playback::StartFirstEver();
}
