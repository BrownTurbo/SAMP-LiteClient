#pragma once
#ifndef DIALOGS_H
#define DIALOGS_H
#include <stdlib.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtGui/QColor>
#include <QtCore/QRegularExpression>
#include <QtCore/QStringList>
#include <QtGui/QKeyEvent>

class SAMPDialog : public QDialog {
    Q_OBJECT

public:
    enum DialogStyle { MSGBOX, INPUT, PASSWORD, LIST, TABLIST, HEADERLIST };

    SAMPDialog(DialogStyle style, QString title, QString info, QString button1, QString button2, QWidget* parent);

signals:
    void dialogResponse(int buttonId, int listitem, const QString& text, bool response);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    DialogStyle dialogStyle;
    QLineEdit* inputField = nullptr;
    QListWidget* listWidget = nullptr;
    QTableWidget* tableWidget = nullptr;

    void setupHeaderList(const QString& info);
    void setupTabList(const QString& info);
    void handleButtonClick(int buttonId) ;

public:
    static SAMPDialog& instance() {
        static SAMPDialog instance; // Single global instance
        return instance;
    }

    bool isDialogVisible() const;
    void closeDialog() ;

private:
    SAMPDialog() = default;
};
#endif
