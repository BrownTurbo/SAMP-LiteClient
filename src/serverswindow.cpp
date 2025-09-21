#include "../include/serverswindow.h"

ServersList::ServersList(QWidget *parent) : QDialog(parent)
{
    this->resize(683, 352);
    this->setFixedSize(683, 352);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
    this->setSizePolicy(sizePolicy);
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ServersLWidget"));

    QVBoxLayout *layout = new QVBoxLayout(this);
    serverL = new QTableWidget(this);
    if (serverL->columnCount() < 3)
        serverL->setColumnCount(3);
    QFont font;
    font.setBold(true);
    font.setWeight(QFont::Weight::Bold);    
    QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
    __qtablewidgetitem->setTextAlignment(Qt::AlignCenter);
    __qtablewidgetitem->setFont(font);
    serverL->setHorizontalHeaderItem(0, __qtablewidgetitem);
    QTableWidgetItem *___qtablewidgetitem = serverL->horizontalHeaderItem(0);
    ___qtablewidgetitem->setText(QString::fromUtf8("Number"));
    QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
    __qtablewidgetitem1->setTextAlignment(Qt::AlignCenter);
    __qtablewidgetitem1->setFont(font);
    serverL->setHorizontalHeaderItem(1, __qtablewidgetitem1);
    QTableWidgetItem *___qtablewidgetitem1 = serverL->horizontalHeaderItem(1);
    ___qtablewidgetitem1->setText(QString::fromUtf8("IP"));
    QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
    __qtablewidgetitem2->setTextAlignment(Qt::AlignCenter);
    __qtablewidgetitem2->setFont(font);
    serverL->setHorizontalHeaderItem(2, __qtablewidgetitem2);
    QTableWidgetItem *___qtablewidgetitem2 = serverL->horizontalHeaderItem(2);
    ___qtablewidgetitem2->setText(QString::fromUtf8("Name"));
    serverL->setObjectName(QString::fromUtf8("serverL"));
    serverL->setGeometry(QRect(0, 4, 301, 347));
    serverL->setSizePolicy(sizePolicy);
    serverL->setFrameShape(QFrame::Box);
    serverL->setFrameShadow(QFrame::Raised);
    serverL->setLineWidth(1);
    serverL->setMidLineWidth(0);
    serverL->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    serverL->horizontalHeader()->setCascadingSectionResizes(false);
    serverLine = new QFrame(this);
    serverLine->setObjectName(QString::fromUtf8("serverLine"));
    serverLine->setGeometry(QRect(302, 280, 371, 71));
    serverLine->setFrameShape(QFrame::HLine);
    serverLine->setFrameShadow(QFrame::Sunken);
    SaveBtn = new QPushButton(this);
    SaveBtn->setObjectName(QString::fromUtf8("SaveBtn"));
    SaveBtn->setGeometry(QRect(560, 320, 80, 30));
    SaveBtn->setFlat(false);
    SaveBtn->setText(QString::fromUtf8("Save"));
    CancelBtn = new QPushButton(this);
    CancelBtn->setObjectName(QString::fromUtf8("CancelBtn"));
    CancelBtn->setGeometry(QRect(470, 320, 80, 30));
    CancelBtn->setFlat(false);
    CancelBtn->setText(QString::fromUtf8("Cancel"));
    srvWidget = new QWidget(this);
    srvWidget->setObjectName(QString::fromUtf8("srvWidget"));
    srvWidget->setGeometry(QRect(307, 10, 336, 301));
    lineEdit = new QLineEdit(srvWidget);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setGeometry(QRect(100, 10, 271, 30));
    lineEdit->setText(QString::fromUtf8("127.0.0.1:7777"));
    label = new QLabel(srvWidget);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(10, 10, 91, 22));
    label->setText(QString::fromUtf8("IP Address:"));
    layout->addWidget(serverL);
    layout->addWidget(srvWidget);
    this->setLayout(layout);

    QMetaObject::connectSlotsByName(this);
}

ServersList::~ServersList() {}
