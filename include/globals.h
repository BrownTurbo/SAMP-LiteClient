#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

#include <QtWidgets/QWidget>
#include <QtWidgets/QWizardPage>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtGui/QStandardItemModel>
#include <QtCore/QSharedPointer>
#include <QtCore/QAtomicPointer>
#include <cstdio>
#include <QtCore/QDebug>
#include <QtCore/QMutex>

class Globals {
public:
    static Globals& instance() {
        static Globals instance;
        return instance;
    }

    QSharedPointer<QWizardPage> getConnectW() const {
        if (ConnectW.isNull())
        {
            qWarning() << "ConnectW is uninitialized!";
            return nullptr;
        }
        return ConnectW;
    }

    void setConnectW(const QSharedPointer<QWizardPage> newWidget) {
        ConnectW = newWidget;
    }

    QSharedPointer<QGroupBox> getSettingsW() const {
        if (SettingsW.isNull())
        {
            qWarning() << "SettingsW is uninitialized!";
            return nullptr;
        }
        return SettingsW;
    }

    void setSettingsW(const QSharedPointer<QGroupBox> newWidget)
    {
        SettingsW = newWidget;
    }

    QSharedPointer<QTextBrowser> getLogsD() const
    {
        if (LogsD.isNull())
        {
            qWarning() << "LogsD is uninitialized!";
            return nullptr;
        }
        return LogsD;
    }

    void setLogsD(const QSharedPointer<QTextBrowser> newB) {
        LogsD = newB;
    }

    QWidget* getLogsW() const
    {
        if (LogsW == nullptr)
        {
            qWarning() << "LogsW is uninitialized!";
            return nullptr;
        }
        return LogsW;
    }

    void setLogsW(QWidget* newW)
    {
        LogsW = newW;
    }

    QSharedPointer<QTextBrowser> getChatD() const {
        if (ChatD.isNull())
        {
            qWarning() << "ChatD is uninitialized!";
            return nullptr;
        }
        return ChatD;
    }

    void setChatD(const QSharedPointer<QTextBrowser> chatD)
    {
        ChatD = chatD;
    }

    FILE* getTXTDLog() const {
        return TXTDLog;
    }

    void setTXTDLog(FILE* TXTD) {
        TXTDLog = TXTD;
    }

    void delTXTDLog() {
        delete TXTDLog;
        TXTDLog = nullptr;
    }

    FILE* getGNRLLog() const {
        return GNRLLog;
    }

    void setGNRLLog(FILE* GNRL) {
        GNRLLog = GNRL;
    }

    void delGNRLLog() {
        delete GNRLLog;
        GNRLLog = nullptr;
    }

    void setMaxPlayers(int maxPlayers) {
        srvMaxPlayers = maxPlayers;
    }

    int getMaxPlayers() const {
        return srvMaxPlayers;
    }

    void setAudioVol(int volume) {
        AudioVolume = volume;
    }

    int getAudioVol() const {
        return AudioVolume;
    }

    QSharedPointer<QLabel> getStateL() const
    {
        if (StateL.isNull()) {
            qWarning() << "StateL is uninitialized!";
            return nullptr;
        }
        qDebug() << "QLabel Pointer:" << static_cast<void *>(StateL.data());
        return StateL;
    }

    void setStateL(const QSharedPointer<QLabel> _stateL)
    {
        QMutexLocker locker(&mutex_);
        StateL = _stateL;
        locker.unlock();
    }

    QWidget* getCentralWidget() const {
        if (CentralWidget == nullptr) {
            qWarning() << "CentralWidget is uninitialized!";
            return nullptr;
        }
        return CentralWidget;
    }

    void setCentralWidget(QWidget* cWidget) {
        QMutexLocker locker(&mutex_);
        CentralWidget = cWidget;
        locker.unlock();
    }

    QSharedPointer<QStandardItemModel> getPropertiesTree() const
    {
        if (propertiesTree.isNull()) {
            qWarning() << "propertiesTree is uninitialized!";
            return nullptr;
        }
        qDebug() << "QStandardItemModel Pointer:" << static_cast<void *>(propertiesTree.data());
        return propertiesTree;
    }

    void setPropertiesTree(const QSharedPointer<QStandardItemModel> _propertiesTree)
    {
        QMutexLocker locker(&mutex_);
        propertiesTree = _propertiesTree;
        locker.unlock();
    }

    QSharedPointer<QLineEdit> getMessageInput() const
    {
        if (msgInput.isNull()) {
            qWarning() << "msgInput is uninitialized!";
            return nullptr;
        }
        qDebug() << "QLineEdit Pointer:" << static_cast<void *>(msgInput.data());
        return msgInput;
    }

    void setMessageInput(const QSharedPointer<QLineEdit> _MessageInput)
    {
        QMutexLocker locker(&mutex_);
        msgInput = _MessageInput;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getSendButton() const
    {
        if (sendBtn.isNull())
        {
            qWarning() << "sendBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(sendBtn.data());
        return sendBtn;
    }

    void setSendButton(const QSharedPointer<QPushButton> _sendButton)
    {
        QMutexLocker locker(&mutex_);
        sendBtn = _sendButton;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getConnectButton() const
    {
        if (connectBtn.isNull())
        {
            qWarning() << "connectBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(connectBtn.data());
        return connectBtn;
    }

    void setConnectButton(const QSharedPointer<QPushButton> _connectButton)
    {
        QMutexLocker locker(&mutex_);
        connectBtn = _connectButton;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getPlaybackButton() const
    {
        if (playbackBtn.isNull())
        {
            qWarning() << "playbackBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(playbackBtn.data());
        return playbackBtn;
    }

    void setPlaybackButton(const QSharedPointer<QPushButton> _playbackBtn)
    {
        QMutexLocker locker(&mutex_);
        playbackBtn = _playbackBtn;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getStopAudioButton() const
    {
        if (stopAudioBtn.isNull())
        {
            qWarning() << "stopAudioBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(stopAudioBtn.data());
        return stopAudioBtn;
    }

    void setStopAudioButton(const QSharedPointer<QPushButton> _stopAudioBtn)
    {
        QMutexLocker locker(&mutex_);
        stopAudioBtn = _stopAudioBtn;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getPauseAudioButton() const
    {
        if (pauseAudioBtn.isNull())
        {
            qWarning() << "pauseAudioBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(pauseAudioBtn.data());
        return pauseAudioBtn;
    }

    void setPauseAudioButton(const QSharedPointer<QPushButton> _pauseAudioBtn)
    {
        QMutexLocker locker(&mutex_);
        pauseAudioBtn = _pauseAudioBtn;
        locker.unlock();
    }

    QSharedPointer<QPushButton> getMuteAudioButton() const
    {
        if (muteAudioBtn.isNull())
        {
            qWarning() << "muteAudioBtn is uninitialized!";
            return nullptr;
        }
        qDebug() << "QPushButton Pointer:" << static_cast<void *>(muteAudioBtn.data());
        return muteAudioBtn;
    }

    void setMuteAudioButton(const QSharedPointer<QPushButton> _muteAudioBtn)
    {
        QMutexLocker locker(&mutex_);
        muteAudioBtn = _muteAudioBtn;
        locker.unlock();
    }

private:
    Globals() : TXTDLog(nullptr), GNRLLog(nullptr), srvMaxPlayers(0), AudioVolume(0), CentralWidget(nullptr) {}
    ~Globals() {}

    Globals(const Globals&) = delete;
    Globals& operator=(const Globals&) = delete;

    QSharedPointer<QWizardPage> ConnectW;
    QSharedPointer<QGroupBox> SettingsW;
    QSharedPointer<QTextBrowser> LogsD;
    QWidget* LogsW;
    QSharedPointer<QTextBrowser> ChatD;
    FILE *TXTDLog;
    FILE *GNRLLog;
    int srvMaxPlayers;
    int AudioVolume;
    QSharedPointer<QLabel> StateL;
    QWidget* CentralWidget;
    QSharedPointer<QStandardItemModel> propertiesTree;
    QSharedPointer<QLineEdit> msgInput;
    QSharedPointer<QPushButton> sendBtn;
    QSharedPointer<QPushButton> connectBtn;
    QSharedPointer<QPushButton> playbackBtn;
    QSharedPointer<QPushButton> stopAudioBtn;
    QSharedPointer<QPushButton> pauseAudioBtn;
    QSharedPointer<QPushButton> muteAudioBtn;

    QMutex mutex_;
};
#endif
