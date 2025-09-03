#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <time.h>
#include <regex>
#include <string>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtCore/QList>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTextBrowser>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtMultimedia/QMediaPlayer>
#include <QtCore/QSharedPointer>
#include <QtCore/QRegularExpression>

#include "common.h"
#include "globals.h"
#include "xmlsets.h"

#include <cstring>

int __MessageBox(const QString title, const QString message, QMessageBox::StandardButtons buttons, QMessageBox::Icon icon, QWidget* widget);
uint32_t GetTickCount();

QWidget* getMainWidget();
QMainWindow* getMainWindow();

int SaveTextDrawData ( uint16_t wTextID, TEXT_DRAW_TRANSMIT *pData, char* cText );
void gen_random(char *s, const int len);
int AppendLogF(const char *fmt, ...);
void AppendChatBox(const char *fmt, ...);
QByteArray readTextFile(const QString &file_path);
QString GetPath(const char *fmt, ...);
bool isAudioPlaying(QMediaPlayer *player);
bool __setStateMessage(const char *text);

QStringList Char2DArrayToQStringList(char **CharTXT, int rowCount);
char *formatColorMessage(const char *szMsg);

void handleProperty(const QString &property, bool removeRow, const QString &value);

char *removeMarkupTags(char *text);
QString removeMarkupTags(QString &text);

bool ToggleChatBox();
bool ToggleSendButton();
bool TogglePlaybackButton();

bool startsWith(const char *text, const char *prefix);

// Thanks to 81Vm3 for RakMagic
const char *getWeaponName(int weaponid);
bool IsVehicleIndexValid(int vehicleid);

void centerOverParent(QWidget* child, QWidget* parent);
