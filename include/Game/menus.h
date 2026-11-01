#pragma once
#ifndef MENUS_H
#define MENUS_H
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGridLayout>
#include <QtCore/QPointer>
#include <QtGui/QCloseEvent>
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
    void closeEvent(QCloseEvent *event) override;

signals:
    void menuResponse(int rowID, bool response);

private:
    int columnCount = 1;
    int currentColumn = 1;
    bool m_responded = false;
    QVBoxLayout *mainLayout = nullptr;
    QHBoxLayout *columnsLayout = nullptr;
    QLabel *titleLabel = nullptr;
    QVBoxLayout *col1Layout = nullptr;
    QLabel *col1HeaderLabel = nullptr;
    QListWidget *col1List = nullptr;
    QVBoxLayout *col2Layout = nullptr;
    QLabel *col2HeaderLabel = nullptr;
    QListWidget *col2List = nullptr;

public:
    static QPointer<SAMPMenu> activeMenu;
    static void closeActiveMenu();

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
