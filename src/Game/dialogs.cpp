#include "../../include/Game/dialogs.h"

QPointer<SAMPDialog> SAMPDialog::activeDialog = nullptr;

bool SAMPDialog::isDialogVisible()
{
    return (activeDialog != nullptr && activeDialog->isVisible());
}

void SAMPDialog::closeActiveDialog()
{
    if (activeDialog)
    {
        activeDialog->close();
        activeDialog = nullptr;
    }
}

bool SAMPDialog::isVisibleDialog() const
{
    return this->isVisible();
}

void SAMPDialog::closeDialog()
{
    closeActiveDialog();
}

SAMPDialog::SAMPDialog(DialogStyle style, QString title, QString info,
            QString button1, QString button2,
            QWidget* parent = nullptr)
    : QDialog(parent), dialogStyle(style), m_responded(false) {
    this->setWindowTitle(title);

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* infoLabel = new QLabel(info, this);
    infoLabel->setWordWrap(true);
    layout->addWidget(infoLabel);

    if (dialogStyle == INPUT || dialogStyle == PASSWORD) {
        inputField = new QLineEdit(this);
        if (dialogStyle == PASSWORD) {
            inputField->setEchoMode(QLineEdit::Password);
        }
        layout->addWidget(inputField);
    } else if (dialogStyle == LIST) {
        listWidget = new QListWidget(this);
        layout->addWidget(listWidget);

        QStringList items = info.split("\n");
        listWidget->addItems(items);
    } else if (dialogStyle == TABLIST) {
        setupTabList(info);
        layout->addWidget(listWidget);
    } else if (dialogStyle == HEADERLIST) {
        setupHeaderList(info);
        layout->addWidget(tableWidget);
    }

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* btn1 = new QPushButton(button1, this);
    buttonLayout->addWidget(btn1);
    this->connect(btn1, &QPushButton::clicked, this, [this]() {handleButtonClick(1); });

    if (!button2.isEmpty()) {
        QPushButton* btn2 = new QPushButton(button2, this);
        buttonLayout->addWidget(btn2);
        this->connect(btn2, &QPushButton::clicked, this, [this]() {handleButtonClick(2); });
    }

    layout->addLayout(buttonLayout);
    this->setLayout(layout);
}

void SAMPDialog::setupHeaderList(const QString& info) {
    QStringList rows = info.split("\n", Qt::SkipEmptyParts);
    if (rows.isEmpty())
        return;

    QStringList headers = rows.first().split("\t");
    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 1; i < rows.size(); ++i) {
        QStringList columns = rows[i].split("\t");
        int rowIndex = tableWidget->rowCount();
        tableWidget->insertRow(rowIndex);

        for (int j = 0; j < columns.size(); ++j) {
            QTableWidgetItem* item = new QTableWidgetItem(columns[j]);

            QRegularExpression colorRegex("\\{([A-Fa-f0-9]{6})\\}");
            QRegularExpressionMatch match = colorRegex.match(columns[j]);
            if (match.hasMatch()) {
                QString colorCode = match.captured(1);
                QColor color("#" + colorCode);
                item->setForeground(color);
                item->setText(columns[j].remove(colorRegex));
            }

            tableWidget->setItem(rowIndex, j, item);
        }
    }
}

void SAMPDialog::setupTabList(const QString& info) {
    listWidget = new QListWidget(this);

    QStringList rows = info.split("\n", Qt::SkipEmptyParts);
    for (const QString& row : rows) {
        QListWidgetItem* item = new QListWidgetItem(row);

        QRegularExpression colorRegex("\\{([A-Fa-f0-9]{6})\\}");
        QRegularExpressionMatch match = colorRegex.match(row);
        if (match.hasMatch()) {
            QString colorCode = match.captured(1);
            QColor color("#" + colorCode);
            item->setForeground(color);
            QString modifiedRow = row;
            item->setText(modifiedRow.remove(colorRegex));
        }

        listWidget->addItem(item);
    }
}

void SAMPDialog::handleButtonClick(int buttonId) {
    if (m_responded)
        return;
    m_responded = true;

    QString text;
    bool response = (buttonId == 1);
    uint8_t sampButtonId = response ? 1 : 0;
    int listBoxItem = 0;

    if (dialogStyle == INPUT || dialogStyle == PASSWORD) {
        listBoxItem = -1;
        if (inputField) {
            text = inputField->text();
        }
    } else if (dialogStyle == MSGBOX) {
        listBoxItem = -1;
        text = "";
    } else if (dialogStyle == LIST || dialogStyle == TABLIST) {
        if (listWidget) {
            int selectedRow = listWidget->currentRow();
            listBoxItem = (selectedRow >= 0) ? selectedRow : 0;
            if (response && listWidget->currentItem())
                text = listWidget->currentItem()->text();
        }
    } else if (dialogStyle == HEADERLIST) {
        if (tableWidget) {
            int selectedRow = tableWidget->currentRow();
            listBoxItem = (selectedRow >= 0) ? selectedRow : 0;
            if (response && selectedRow >= 0) {
                QStringList colTexts;
                for (int c = 0; c < tableWidget->columnCount(); ++c) {
                    QTableWidgetItem *item = tableWidget->item(selectedRow, c);
                    colTexts.append(item ? item->text() : QString());
                }
                text = colTexts.join('\t');
            }
        }
    }

    emit dialogResponse(sampButtonId, listBoxItem, text, response);
    this->close();
}

void SAMPDialog::closeEvent(QCloseEvent *event)
{
    if (!m_responded)
    {
        m_responded = true;
        emit dialogResponse(0, -1, QString(), false);
    }
    if (activeDialog == this)
        activeDialog = nullptr;
    QDialog::closeEvent(event);
}

void SAMPDialog::keyPressEvent(QKeyEvent *event)
{
    int currentRow = 0;
    if (dialogStyle == LIST) {
        if (listWidget && listWidget->currentItem())
            currentRow = listWidget->currentRow();
    } else if (dialogStyle == TABLIST) {
        if (listWidget) {
            int selectedRow = listWidget->currentRow();
            currentRow = (selectedRow >= 0) ? selectedRow : 0;
        }
    } else if (dialogStyle == HEADERLIST) {
        if (tableWidget) {
            auto selectedItems = tableWidget->selectedItems();
            if (!selectedItems.isEmpty())
            {
                currentRow = tableWidget->row(selectedItems.first());
                if (currentRow < 0)
                    currentRow = 0;
            }
            else
                currentRow = 0;
        }
    }

    switch (event->key())
    {
        case Qt::Key_Up: {
            if (currentRow > 0) {
                if (dialogStyle == LIST || dialogStyle == TABLIST)
                {
                    if(listWidget)
                        listWidget->setCurrentRow(currentRow - 1);
                }
                else if (dialogStyle == HEADERLIST) {
                    if (tableWidget)
                        tableWidget->selectRow(currentRow - 1);
                }
            }
            break;
        }
        case Qt::Key_Down: {
            if (dialogStyle == LIST || dialogStyle == TABLIST)
            {
                if (listWidget && currentRow < listWidget->count() - 1)
                    listWidget->setCurrentRow(currentRow + 1);
            }
            else if (dialogStyle == HEADERLIST)
            {
                if (tableWidget && currentRow < tableWidget->rowCount() - 1)
                    tableWidget->selectRow(currentRow + 1);
            }
            break;
        }
        case Qt::Key_Escape: {
            if (!m_responded)
            {
                m_responded = true;
                emit dialogResponse(0, -1, QString(), false);
            }
            this->close();
            break;
        }
        case Qt::Key_Return:
        case Qt::Key_Enter: {
            handleButtonClick(1);
            break;
        }
        default:
            QWidget::keyPressEvent(event);
    }
}
