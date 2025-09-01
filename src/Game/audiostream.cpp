#include "../../include/Game/audiostream.h"

bool AudioStream::PlayAudioStream(QString audioUrl, qreal volume)
{
    if (mediaPlayer != nullptr && isAudioPlaying(mediaPlayer))
        StopAudioStream();
    bool _RET = true;

    if (audioOutput == nullptr)
        audioOutput = new QAudioOutput;
    audioOutput->setVolume(volume);

    mediaPlayer = new QMediaPlayer;
    mediaPlayer->setAudioOutput(audioOutput);

    QObject::connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, [](QMediaPlayer::MediaStatus status)
                     {
        if (status == QMediaPlayer::BufferedMedia) {
            AppendLogF("Audio buffering complete, playback started.");
        } else if (status == QMediaPlayer::EndOfMedia) {
            AppendLogF("Audio playback finished.");
        } });

    QObject::connect(mediaPlayer, &QMediaPlayer::playbackStateChanged, [&](QMediaPlayer::PlaybackState state)
                     {
        if (state == QMediaPlayer::StoppedState) {
            AppendLogF("Playback stopped. Audio stream has ended.");
        } });

    QObject::connect(mediaPlayer, &QMediaPlayer::errorOccurred, [&](QMediaPlayer::Error error, const QString &errorString)
                     {
        AppendLogF("Error occurred:", errorString);
        _logs->Log(LogLevel::ERROR, "Error occurred:", errorString);
        _RET = false; });

    mediaPlayer->setSource(QUrl(audioUrl));

    mediaPlayer->play();
    AppendLogF("[RPC]-> Playing audio from:", audioUrl);
    _logs->Log(LogLevel::INFO, "[RPC]-> Playing audio from:", audioUrl);

    return _RET;
}

bool AudioStream::StopAudioStream()
{
    if (mediaPlayer == nullptr)
        return false;
    AppendLogF("[RPC]-> StopAudioStream.");
    _logs->Log(LogLevel::INFO, "[RPC]-> StopAudioStream.");
    mediaPlayer->stop();
    return (mediaPlayer->playbackState() == QMediaPlayer::StoppedState);
}

bool AudioStream::PauseAudioStream()
{
    if (mediaPlayer == nullptr)
        return false;
    AppendLogF("[REQ]-> PauseAudioStream.");
    _logs->Log(LogLevel::INFO, "[RPC]-> PauseAudioStream.");
    mediaPlayer->pause();
    return (mediaPlayer->playbackState() == QMediaPlayer::PausedState);
}

qreal AudioStream::GetSysAudioVolume()
{
    QAudioDevice defaultDevice = QMediaDevices::defaultAudioInput();
    QAudioInput audioInput(defaultDevice);

    return audioInput.volume();
}

qreal AudioStream::GetAudioVolume() const
{
    if (mediaPlayer == nullptr)
        return 0.0;
    if (mediaPlayer->playbackState() != QMediaPlayer::PlayingState && mediaPlayer->playbackState() != QMediaPlayer::PausedState)
        return 0.0;

    qreal audioVol = 0.0;
    if (audioOutput != nullptr)
        audioVol = audioOutput->volume();
    return audioVol;
}

bool AudioStream::SetAudioVolume(qreal volume)
{
    if (mediaPlayer == nullptr)
        return false;
    if (mediaPlayer->playbackState() != QMediaPlayer::PlayingState && mediaPlayer->playbackState() != QMediaPlayer::PausedState)
        return false;

    if (audioOutput != nullptr)
        audioOutput->setVolume(volume);
    return true;
}
