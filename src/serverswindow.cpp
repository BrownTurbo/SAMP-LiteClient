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

    QVBoxLayout *VLayout = new QVBoxLayout(this);
    VLayout->setGeometry(QRect(0, 0, 683, 352));
    QHBoxLayout *HLayout_ = new QHBoxLayout(this);
    HLayout_->setGeometry(QRect(0, 1, 351, 348));
    QHBoxLayout *HLayout = new QHBoxLayout(this);
    HLayout_->setGeometry(QRect(0, 320, 300, 60));
    HLayout->addStretch();
    serverL = new QTableWidget(this);
    if (serverL->columnCount() < 3)
        serverL->setColumnCount(3);
    QFont font;
    font.setBold(true);
    font.setWeight(QFont::Weight::Bold);
    QTableWidgetItem *__widgetItem = new QTableWidgetItem();
    __widgetItem->setTextAlignment(Qt::AlignCenter);
    __widgetItem->setFont(font);
    serverL->setHorizontalHeaderItem(0, __widgetItem);
    QTableWidgetItem *___widgetItem = serverL->horizontalHeaderItem(0);
    ___widgetItem->setText(QString::fromUtf8("Number"));
    QTableWidgetItem *__widgetItem1 = new QTableWidgetItem();
    __widgetItem1->setTextAlignment(Qt::AlignCenter);
    __widgetItem1->setFont(font);
    serverL->setHorizontalHeaderItem(1, __widgetItem1);
    QTableWidgetItem *___widgetItem1 = serverL->horizontalHeaderItem(1);
    ___widgetItem1->setText(QString::fromUtf8("IP"));
    QTableWidgetItem *__widgetItem2 = new QTableWidgetItem();
    __widgetItem2->setTextAlignment(Qt::AlignCenter);
    __widgetItem2->setFont(font);
    serverL->setHorizontalHeaderItem(2, __widgetItem2);
    QTableWidgetItem *___widgetItem2 = serverL->horizontalHeaderItem(2);
    ___widgetItem2->setText(QString::fromUtf8("Name"));
    serverL->setObjectName(QString::fromUtf8("serverL"));
    serverL->setGeometry(QRect(0, 4, 305, 347));
    serverL->setFixedWidth(305);
    serverL->setFrameShape(QFrame::Box);
    serverL->setFrameShadow(QFrame::Raised);
    serverL->setLineWidth(1);
    serverL->setMidLineWidth(0);
    serverL->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
    serverL->horizontalHeader()->setCascadingSectionResizes(false);
    serverL->setAutoScroll(true);
    serverL->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    serverL->setEditTriggers(QAbstractItemView::NoEditTriggers);
    serverL->setSelectionBehavior(QAbstractItemView::SelectRows);
    serverL->setSelectionMode(QAbstractItemView::SingleSelection);
    serverL->verticalHeader()->setVisible(false);
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
    SaveBtn->setCursor(QCursor(Qt::PointingHandCursor));
    CancelBtn = new QPushButton(this);
    CancelBtn->setObjectName(QString::fromUtf8("CancelBtn"));
    CancelBtn->setGeometry(QRect(470, 320, 80, 30));
    CancelBtn->setFlat(false);
    CancelBtn->setText(QString::fromUtf8("Cancel"));
    CancelBtn->setCursor(QCursor(Qt::PointingHandCursor));
    lineEdit = new QLineEdit(this);
    lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
    lineEdit->setGeometry(QRect(150, 10, 271, 30));
    lineEdit->setFixedWidth(250);
    lineEdit->setFixedHeight(30);
    lineEdit->setText(QString::fromUtf8("127.0.0.1:7777"));
    label = new QLabel(this);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(25, 10, 91, 22));
    label->setFixedWidth(91);
    label->setFixedHeight(22);
    label->setText(QString::fromUtf8("IP Address:"));
    HLayout_->addWidget(serverL, 1, Qt::AlignmentFlag::AlignTop);
    HLayout_->addWidget(label, 0, Qt::AlignmentFlag::AlignTop);
    HLayout_->addWidget(lineEdit, 0, Qt::AlignmentFlag::AlignTop);
    VLayout->addLayout(HLayout_);
    HLayout->addWidget(serverLine);
    HLayout->addWidget(SaveBtn);
    HLayout->addWidget(CancelBtn);
    VLayout->addLayout(HLayout);
    this->setLayout(VLayout);

    QMetaObject::connectSlotsByName(this);
}

ServersList::~ServersList() {}
