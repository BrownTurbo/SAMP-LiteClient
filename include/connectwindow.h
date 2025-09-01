#pragma once
#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWizardPage>

//#include "globals.h"
#include "utils.h"
#include "common.h"

class ConnectWindow : public QWizardPage {
    Q_OBJECT

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

private:
    void onConnectPressed();
    void onCancelPressed();

private:
    QLineEdit *SPasswordBox;
    QLineEdit *RPasswordBox;
    QLabel *SPasswordTXT;
    QLabel *RpasswordTXT;
    QPushButton *connectButton;
    QPushButton *cancelButton;
    QGroupBox *groupBox;
    QLabel *SName;
    QLabel *SIP;
    QLabel *SIPValue;
    QLabel *SNameValue;
};

#endif // CONNECTWINDOW_H
