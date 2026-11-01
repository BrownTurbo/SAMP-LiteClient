#pragma once
#ifndef AUDIOS_H
#define AUDIOS_H

#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QAudioDevice>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QMediaDevices>
#include <QtCore/QUrl>

#include "../utils.h"

class AudioStream {
public:
    AudioStream();
    ~AudioStream();

    static AudioStream& instance() {
        static AudioStream s_instance;
        return s_instance;
    }

    qreal *audioVolume = nullptr;

public:
    bool PlayAudioStream(QString audioUrl, qreal volume);
    bool StopAudioStream();
    bool PauseAudioStream();

    qreal GetAudioVolume() const;
    bool SetAudioVolume(qreal volume);

private:
    QAudioOutput *audioOutput = nullptr;
    QMediaPlayer *mediaPlayer = nullptr;

private:
    qreal GetSysAudioVolume();
};
#endif
