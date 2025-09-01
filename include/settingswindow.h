#pragma once
#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>

#include "globals.h"

class SettingsWindow : public QGroupBox {
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    QGroupBox *gamesec;
    QGroupBox *playersec;
    QLineEdit *playerName;
    QLabel *playerNameL;
    QLineEdit *serverIP;
    QLabel *serverIPL;
    QPushButton *SaveButton;
    QPushButton *CancelButton;
    QSlider *aStreamS;
    QLabel *aStreamL;

protected:
    void onVolumeChanged(int value);
    void onSavePressed();
    void onCancelPressed();
};

#endif // SETTINGSWINDOW_H
