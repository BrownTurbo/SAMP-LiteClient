#include "../../include/Game/menus.h"

SAMPMenu::SAMPMenu(QString title, int columns, QStringList Col1Items, bool *Col1ItemsState, QString Col1Header, QStringList Col2Items, bool *Col2ItemsState, QString Col2Header, QWidget *parent)
    : QWidget(parent), columnCount(columns > 2 ? 2 : columns)
{
    if (Col1Items.size() == 0 || Col1Items.size() > MAX_MENU_ITEMS || Col2Items.size() > MAX_MENU_ITEMS)
        return;
    this->setFixedSize(400, 300);

    mainLayout = new QVBoxLayout(this);

    titleLabel = new QLabel(this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; padding: 10px;");
    titleLabel->setText(title);
    titleLabel->setWordWrap(true);
    mainLayout->addWidget(titleLabel);

    columnsLayout = new QHBoxLayout;

    col1Layout = new QVBoxLayout;
    col1HeaderLabel = new QLabel(Col1Header, this);
    col1HeaderLabel->setAlignment(Qt::AlignCenter);
    col1HeaderLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
    col1Layout->addWidget(col1HeaderLabel);

    col1List = new QListWidget(this);
    col1List->setStyleSheet("font-size: 16px;");
    col1List->setWrapping(true);
    col1List->setSpacing(5);
    col1List->setMovement(QListView::Static);
    QListWidgetItem *item;
    for (int i = 0; i < Col1Items.size(); ++i)
    {
        if (Col1ItemsState[i])
        {
            item = new QListWidgetItem(Col1Items.at(i), col1List);
            item->setTextAlignment(Qt::AlignCenter);
        }
    }
    col1List->setCurrentRow(0);
    col1Layout->addWidget(col1List);
    columnsLayout->addLayout(col1Layout);

    if (columnCount > 1)
    {
        col2Layout = new QVBoxLayout;
        col2HeaderLabel = new QLabel(Col2Header, this);
        col2HeaderLabel->setAlignment(Qt::AlignCenter);
        col2HeaderLabel->setStyleSheet("font-size: 18px; font-weight: bold;");
        col2Layout->addWidget(col2HeaderLabel);

        col2List = new QListWidget(this);
        col2List->setStyleSheet("font-size: 16px;");
        col2List->setWrapping(true);
        col2List->setSpacing(5);
        col2List->setMovement(QListView::Static);
        for (int i = 0; i < Col2Items.size(); ++i)
        {
            if (Col2ItemsState[i])
            {
                item = new QListWidgetItem(Col2Items.at(i), col2List);
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
        col2List->setCurrentRow(0);
        col2Layout->addWidget(col2List);
        columnsLayout->addLayout(col2Layout);
    }

    mainLayout->addLayout(columnsLayout);

    this->setLayout(mainLayout);

    this->connect(col1List, &QListWidget::itemClicked, this, [=, this] {
        currentColumn = 1;
    });

    this->connect(col2List, &QListWidget::itemClicked, this, [=, this]
                  { currentColumn = 2; });

    currentColumn = 1;
}

bool SAMPMenu::isMenuVisible() const
{
    return this->isVisible();
}

void SAMPMenu::closeMenu()
{
    if (this->isVisible())
        this->close();
}

void SAMPMenu::keyPressEvent(QKeyEvent *event)
{
    QListWidget *activeList = (currentColumn == 1) ? col1List : col2List;
    int currentRow = activeList->currentRow();

    switch (event->key())
    {
        case Qt::Key_Up: {
            if (currentRow > 0)
                activeList->setCurrentRow(currentRow - 1);
            break;
        }
        case Qt::Key_Down: {
            if (currentRow < activeList->count() - 1)
                activeList->setCurrentRow(currentRow + 1);
            break;
        }
        case Qt::Key_Left: {
            if (currentColumn == 2) {
                col1List->setFocus();
                currentColumn = 1;
                col1List->setCurrentRow(currentRow < col1List->count() ? currentRow : col1List->count() - 1);
            }
            break;
        }
        case Qt::Key_Right: {
            if (currentColumn == 1 && columnCount > 1) {
                col2List->setFocus();
                currentColumn = 2;
                col2List->setCurrentRow(currentRow < col2List->count() ? currentRow : col2List->count() - 1);
            }
            break;
        }
        case Qt::Key_Escape: {
            emit menuResponse(-1, false);
            break;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            emit menuResponse(activeList->currentRow(), true);
            break;
        }
        default:
            QWidget::keyPressEvent(event);
    }
}
