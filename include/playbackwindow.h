#pragma once
#ifndef PLAYBACKW_H
#define PLAYBACKW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTimer>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <cstdlib>
#include <ctime>
#include <time.h>

#include "common.h"
#include "utils.h"
#include "globals.h"
#include "WorkerClass.h"

#include "Game/playback.h"
#include "Game/recorder.h"

#define MAX_RECORDS 100

class PlaybackWindow : public QGroupBox
{
    Q_OBJECT

public:
    PlaybackWindow(QWidget *parent = nullptr);
    ~PlaybackWindow();

private:
    QTableWidget *tableWidget;
    QPushButton *playbtn;
    QPushButton *pausebtn;
    QPushButton *recordbtn;
    QPushButton *closebtn;
    QLabel *statuslabel;
    QComboBox *recordType;

private:
    char szInput[128];
    char PlaybackFile[50];
    char szFileSaveAs[128];

    QTimer *UpdStatusTimer;

    char RECfileN[50];

    struct recData
    {
        QString recF;
        char *recTitle;
        VECTOR recPos[1024];
        Playback::ePBType recType;
    };

    recData recordsData[MAX_RECORDS];

protected:
    void updateStatusElem();
    void onPlayPressed();
    void onPausePressed();
    void onRecordPressed();
    void onClosePressed();

public:
    void LoadPlayback(bool autoPlay = false);
    void SavePlayback(char szFileName[128]);
    void StartRecording(int iRecordType = Playback::PLAYBACK_TYPE_ONFOOT);
    void TogglePlayback();
};
#endif
