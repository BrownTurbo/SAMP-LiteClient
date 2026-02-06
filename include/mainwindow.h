#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMessageBox>
#include <QtCore/QTimer>
#include <QtMultimedia/QAudioDevice>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

#include <memory>
#include <chrono>

#include "logswindow.h"
#include "settingswindow.h"
#include "connectwindow.h"
#include "playbackwindow.h"
#include "serverswindow.h"
#include "scorebwindow.h"

#include "globals.h"
#include "xmlsets.h"
#include "utils.h"
#include "logs.h"

#include "Game/audiostream.h"

#include "ThreadWorker.h"
#include "RakNetWorker.h"
#include "WorkerClass.h"

#include "IntervalWorker.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLogsWindow();
    void showSettingsWindow();
    void showConnectWindow();
    void showScoreBoardWindow();
    void showPlaybackWindow();
    void showServerLWindow();

    void onAudioPauseClicked();
    void onAudioStopClicked();
    void onMuteClicked();

public:
    void sendMessage();

private:
    QTextBrowser *chatDisplay;
    QLineEdit *messageInput;
    QPushButton *sendButton;
    QProgressBar *stateProgressBar;
    QPushButton *toolButton;
    QPushButton *rootbtn;
    QTreeView *treeView;
    QStandardItemModel* treeVmodel;
    QLabel *stateMsg;
    //QFrame *line;
    QPushButton *logsButton;
    QPushButton *scoreboardbtn;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *audioCButton;
    QPushButton *actionsButton;
    QPushButton *playbackbtn;
    QPushButton *serversList;

    LogsWindow *logsWindow = nullptr;
    SettingsWindow *settingsWindow = nullptr;
    ConnectWindow *connectWindow = nullptr;
    PlaybackWindow *playbackWindow = nullptr;
    Scoreboard *scoreboardWindow = nullptr;
    ServersList *serverlstWindow = nullptr;

    RakNetWorker *Nworker;

    WorkerClass *worker;
    QThread *workerThread;

    qreal lastAudioLVL;
};

#endif // MAINWINDOW_H
