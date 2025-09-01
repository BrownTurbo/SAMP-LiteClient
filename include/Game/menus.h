#pragma once
#ifndef MENUS_H
#define MENUS_H
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGridLayout>
#include <QtGui/QKeyEvent>
#include <QtCore/QDebug>
#include <QtGui/QFocusEvent>

#include "../common.h"

class SAMPMenu : public QWidget
{
    Q_OBJECT

public:
    SAMPMenu(QString title, int columns, QStringList Col1Items, bool* Col1ItemsState, QString Col1Header, QStringList Col2Items, bool* Col2ItemsState, QString Col2Header, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void menuResponse(int rowID, bool response);

private:
    int columnCount;
    int currentColumn;
    QVBoxLayout *mainLayout;
    QHBoxLayout *columnsLayout;
    QLabel *titleLabel;
    QVBoxLayout *col1Layout;
    QLabel *col1HeaderLabel;
    QListWidget *col1List;
    QVBoxLayout *col2Layout;
    QLabel *col2HeaderLabel;
    QListWidget *col2List;

public:
    static SAMPMenu& instance() {
        static SAMPMenu instance; // Single global instance
        return instance;
    }

    bool isMenuVisible() const;
    void closeMenu();

private:
    SAMPMenu() = default;
};
#endif
