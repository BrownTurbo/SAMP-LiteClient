#include "connectwindow.h"

ConnectWindow::ConnectWindow(QWidget *parent) : QWizardPage(parent) {
        if (this->objectName().isEmpty())
            this->setObjectName(QString::fromUtf8("ConnectWindow"));

        //Globals::instance().setConnectW(this);

        this->resize(329, 233);
        this->setFixedSize(QSize(329, 233));

        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
        this->setSizePolicy(sizePolicy);
        SPasswordBox = new QLineEdit(this);
        SPasswordBox->setObjectName(QString::fromUtf8("SPasswordBox"));
        SPasswordBox->setGeometry(QRect(70, 100, 251, 30));
        RPasswordBox = new QLineEdit(this);
        RPasswordBox->setObjectName(QString::fromUtf8("RPasswordBox"));
        RPasswordBox->setGeometry(QRect(115, 140, 205, 30));
        SPasswordTXT = new QLabel(this);
        SPasswordTXT->setObjectName(QString::fromUtf8("SPasswordTXT"));
        SPasswordTXT->setGeometry(QRect(10, 100, 61, 22));
        RpasswordTXT = new QLabel(this);
        RpasswordTXT->setObjectName(QString::fromUtf8("RpasswordTXT"));
        RpasswordTXT->setGeometry(QRect(10, 140, 106, 22));
        connectButton = new QPushButton(this);
        connectButton->setObjectName(QString::fromUtf8("connectButton"));
        connectButton->setGeometry(QRect(10, 190, 80, 30));
        connectButton->setEnabled(true);
        connectButton->setCursor(Qt::PointingHandCursor);
        cancelButton = new QPushButton(this);
        cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
        cancelButton->setGeometry(QRect(240, 190, 80, 30));
        cancelButton->setEnabled(true);
        cancelButton->setCursor(Qt::PointingHandCursor);
        groupBox = new QGroupBox(this);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 10, 311, 80));
        SName = new QLabel(groupBox);
        SName->setObjectName(QString::fromUtf8("SName"));
        SName->setGeometry(QRect(10, 30, 41, 22));
        SIP = new QLabel(groupBox);
        SIP->setObjectName(QString::fromUtf8("SIP"));
        SIP->setGeometry(QRect(10, 50, 21, 22));
        SIPValue = new QLabel(groupBox);
        SIPValue->setObjectName(QString::fromUtf8("SIPValue"));
        SIPValue->setGeometry(QRect(30, 50, 271, 22));
        SNameValue = new QLabel(groupBox);
        SNameValue->setObjectName(QString::fromUtf8("SNameValue"));
        SNameValue->setGeometry(QRect(50, 30, 251, 22));

        this->connect(connectButton, &QPushButton::clicked, this, &ConnectWindow::onConnectPressed);
        this->connect(cancelButton, &QPushButton::clicked, this, &ConnectWindow::onCancelPressed);

        this->setWindowTitle(QString::fromUtf8("Connect"));
        SPasswordTXT->setText(QString::fromUtf8("Password:"));
        SPasswordTXT->setStyleSheet("background-color: white; border: 0.5px solid #BDBDBD; border-radius: 0.5px; margin-left: -1px;");
        RpasswordTXT->setText(QString::fromUtf8("RCON Password:"));
        RpasswordTXT->setStyleSheet("background-color: white; border: 0.5px solid #BDBDBD; border-radius: 0.5px; margin-left: -1px;");
        connectButton->setText(QString::fromUtf8("Connect"));
        cancelButton->setText(QString::fromUtf8("Cancel"));
        groupBox->setTitle(QString::fromUtf8("Information"));
        SName->setText(QString::fromUtf8("Name:"));
        SIP->setText(QString::fromUtf8("IP:"));
        SIPValue->setText(QString::fromUtf8("127.0.0.1:7777"));
        SNameValue->setText(QString::fromUtf8("BrownTurbo's Extreme Evolution"));

        QMetaObject::connectSlotsByName(this);
}

ConnectWindow::~ConnectWindow() {}

void ConnectWindow::onConnectPressed()
{
    if (GetGameState() != GAMESTATE_CONNECTED)
    {
        QSharedPointer<QPushButton> _connectBtn = Globals::instance().getSendButton();
        if (_connectBtn.isNull())
        {
            qWarning() << "_connectBtn is not a valid pointer";
            return;
        }
        QPushButton *connectBtn = _connectBtn.get();
        if (connectBtn == nullptr || connectBtn->parent() == nullptr)
        {
            qWarning() << "QPushButton instance is not valid.";
            return;
        }
        SetGameState(GAMESTATE_CONNECTION_REQUESTED);
        AppendChatBox("Connection Requested.");
        AppendLogF("Connection Requested.");
        _logs->Log(LogLevel::INFO, "Connection Requested.");
        connectBtn->setText("Stop");
        this->close();
    }
}

void ConnectWindow::onCancelPressed()
{
    this->close();
}
