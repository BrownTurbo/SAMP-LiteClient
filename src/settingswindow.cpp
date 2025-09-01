#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) : QGroupBox(parent) {
        if (this->objectName().isEmpty())
            this->setObjectName(QString::fromUtf8("SettingsBox"));

        //Globals::instance().setSettingsW(this);

        this->resize(500, 500);
        this->setMinimumSize(QSize(455, 338));
        this->setMaximumSize(QSize(500, 500));

        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());

        gamesec = new QGroupBox(this);
        if (gamesec->objectName().isEmpty())
            gamesec->setObjectName(QString::fromUtf8("gamesec"));
        gamesec->setMinimumSize(QSize(271, 441));
        gamesec->setMaximumSize(QSize(271, 441));
        sizePolicy.setHeightForWidth(gamesec->sizePolicy().hasHeightForWidth());
        gamesec->setSizePolicy(sizePolicy);
        gamesec->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        gamesec->setFlat(true);

        playersec = new QGroupBox(this);
        if (playersec->objectName().isEmpty())
            playersec->setObjectName(QString::fromUtf8("playersec"));
        playersec->setMinimumSize(QSize(221, 441));
        playersec->setMaximumSize(QSize(221, 441));
        sizePolicy.setHeightForWidth(playersec->sizePolicy().hasHeightForWidth());
        playersec->setSizePolicy(sizePolicy);
        playersec->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        playersec->setFlat(true);

        QWidget *gameWidget = new QWidget(gamesec);
        gameWidget->setMinimumSize(QSize(300, 200));

        QWidget *playerWidget = new QWidget(playersec);
        playerWidget->setMinimumSize(QSize(300, 200));

        playerName = new QLineEdit(playerWidget);
        if (playerName->objectName().isEmpty())
            playerName->setObjectName(QString::fromUtf8("playerName"));
        playerName->setGeometry(QRect(60, 3, 151, 30));
        playerNameL = new QLabel(playerWidget);
        if (playerNameL->objectName().isEmpty())
            playerNameL->setObjectName(QString::fromUtf8("playerNameL"));
        playerNameL->setGeometry(QRect(10, 3, 51, 31));
        serverIP = new QLineEdit(playerWidget);
        if (serverIP->objectName().isEmpty())
            serverIP->setObjectName(QString::fromUtf8("serverIP"));
        serverIP->setGeometry(QRect(70, 41, 141, 30));
        serverIPL = new QLabel(playerWidget);
        if (serverIPL->objectName().isEmpty())
            serverIPL->setObjectName(QString::fromUtf8("serverIPL"));
        serverIPL->setGeometry(QRect(10, 41, 61, 31));

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();
        SaveButton = new QPushButton(this);
        if (SaveButton->objectName().isEmpty())
            SaveButton->setObjectName(QString::fromUtf8("SaveButton"));
        SaveButton->setGeometry(QRect(10, 460, 71, 30));
        SaveButton->setEnabled(true);
        SaveButton->setCursor(Qt::PointingHandCursor);
        buttonLayout->addWidget(SaveButton);
        CancelButton = new QPushButton(this);
        if (CancelButton->objectName().isEmpty())
            CancelButton->setObjectName(QString::fromUtf8("CancelButton"));
        CancelButton->setGeometry(QRect(420, 460, 71, 30));
        CancelButton->setEnabled(true);
        CancelButton->setCursor(Qt::PointingHandCursor);
        buttonLayout->addWidget(CancelButton);

        aStreamS = new QSlider(gameWidget);
        if (aStreamS->objectName().isEmpty())
            aStreamS->setObjectName(QString::fromUtf8("aStreamS"));
        aStreamS->setGeometry(QRect(100, 3, 161, 20));
        aStreamS->setOrientation(Qt::Horizontal);
        aStreamS->setRange(0, 100);
        aStreamS->setValue(0);
        aStreamS->setToolTip("AudioStream");
        connect(aStreamS, &QSlider::valueChanged, this, &SettingsWindow::onVolumeChanged);
        aStreamL = new QLabel(gameWidget);
        if (aStreamL->objectName().isEmpty())
            aStreamL->setObjectName(QString::fromUtf8("aStreamL"));
        aStreamL->setGeometry(QRect(10, 3, 81, 22));

        QTabWidget *tabWidget = new QTabWidget(this);
        tabWidget->addTab(playerWidget, "Player");
        tabWidget->addTab(gameWidget, "Game");

        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addWidget(tabWidget);
        mainLayout->addLayout(buttonLayout);
        this->setLayout(mainLayout);

        this->setWindowTitle(QString::fromUtf8("Settings"));
        playerNameL->setText(QString::fromUtf8("Name:"));
        playerNameL->setStyleSheet("background-color: white; border: 0.5px solid #BDBDBD; border-radius: 0.5px; margin-left: -1px; font-size:11px;");
        serverIPL->setText(QString::fromUtf8("Server IP:"));
        serverIPL->setStyleSheet("background-color: white; border: 0.5px solid #BDBDBD; border-radius: 0.5px; margin-left: -1px; font-size:11px;");
        aStreamL->setText(QString::fromUtf8("Audio Stream:"));
        aStreamL->setStyleSheet("background-color: white; border: 0.5px solid #BDBDBD; border-radius: 0.5px; margin-left: -1px; font-size:11px;");
        SaveButton->setText(QString::fromUtf8("Save"));
        CancelButton->setText(QString::fromUtf8("Cancel"));

        this->connect(SaveButton, &QPushButton::clicked, this, &SettingsWindow::onSavePressed);
        this->connect(CancelButton, &QPushButton::clicked, this, &SettingsWindow::onCancelPressed);

        QMetaObject::connectSlotsByName(this);
}

SettingsWindow::~SettingsWindow() {}


void SettingsWindow::onVolumeChanged(int value) {
    //
}

void SettingsWindow::onSavePressed() {
    Globals::instance().setAudioVol(aStreamS->value());
}

void SettingsWindow::onCancelPressed() {
    this->close();
}
