#include "../include/utils.h"

int __MessageBox(const QString title, const QString message, QFlags<QMessageBox::StandardButton> buttons, QMessageBox::Icon icon, QWidget* widget = nullptr) {
    if (QApplication::instance()->thread() != QThread::currentThread())
        qDebug() << "> in MessageBox: Thread is not in main thread...";
    QWidget* mainW;
    mainW = (widget == nullptr ? QApplication::focusWidget() : widget);
    QApplication::alert(mainW, 0);
    QMessageBox messageBox(mainW);
    messageBox.setWindowTitle(title);
    messageBox.setText(message);
    messageBox.setStandardButtons(buttons);
    messageBox.setIcon(icon);

    return messageBox.exec();
}

uint32_t GetTickCount() {
    using namespace std::chrono;
    return static_cast<uint32_t>(duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()).count());
}

QWidget* getMainWidget() {
    const QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();
    return topLevelWidgets.isEmpty() ? nullptr : topLevelWidgets.first();
}

QMainWindow* getMainWindow() {
    // Get the list of top-level widgets
    const QList<QWidget*> topLevelWidgets = QApplication::topLevelWidgets();

    // Find and return the first QMainWindow
    for (QWidget* widget : topLevelWidgets) {
        QMainWindow* mainWindow = qobject_cast<QMainWindow*>(widget);
        if (mainWindow)
            return mainWindow;
    }
    return nullptr; // No QMainWindow found
}

int AppendLogF(const char *fmt, ...)
{
    if (ROOT_PATH == nullptr || strlen(ROOT_PATH) == 0)
    {
        fprintf(stderr, "ROOT_PATH is invalid\n");
        return -1;
    }
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%s/logs/client_log.txt", ROOT_PATH);

    char buffer[1024 + 1];
	memset(buffer, 0, sizeof(buffer));

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

    FILE* flLog = Globals::instance().getGNRLLog();
    if (flLog == nullptr) {
        flLog = fopen(filePath, "a");
        Globals::instance().setGNRLLog(flLog);
        if (flLog == nullptr) {
            perror("Error opening file for appending");
            return 0;
        }
    }

    struct tm* st;
    time_t curr_time = time(NULL);
	st = localtime(&curr_time);

	fprintf(flLog, "[%02d-%02d-%02d %02d:%02d:%02d] ",  (st->tm_year + 1900) , (st->tm_mon + 1) , st->tm_mday, st->tm_hour, st->tm_min, st->tm_sec);

    if (fputs(buffer, flLog) == EOF) {
        perror("Error writing to file");
        fclose(flLog);
        Globals::instance().delGNRLLog();
        return 0;
    }

	fprintf(flLog, "\n");

	fflush(flLog);

    //fclose(flLog);
    return 1;
}

void AppendChatBox(const char *fmt, ...)
{
    char buffer[1024 + 1];
	memset(buffer, 0, sizeof(buffer));

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

    QSharedPointer<QTextBrowser> _chatDisplay = Globals::instance().getChatD();
    QTextBrowser *chatDisplay = _chatDisplay.data();
    QMetaObject::invokeMethod(getMainWindow(), [chatDisplay, buffer]() {
        chatDisplay->append(buffer);
    }, Qt::QueuedConnection);
    return;
}

int SaveTextDrawData ( uint16_t wTextID, TEXT_DRAW_TRANSMIT *pData, char* cText )
{
    if (ROOT_PATH == nullptr || strlen(ROOT_PATH) == 0)
    {
        fprintf(stderr, "ROOT_PATH is invalid\n");
        return -1;
    }
    char filePath[256];
    snprintf(filePath, sizeof(filePath), "%s/logs/Textdraws.data", ROOT_PATH);

    FILE* flTextDrawsLog =  Globals::instance().getTXTDLog();
	if (flTextDrawsLog == nullptr)
	{
		flTextDrawsLog = fopen(filePath, "a" );
		Globals::instance().setTXTDLog(flTextDrawsLog);

		if (flTextDrawsLog == nullptr)
			return 0;
	}

	fprintf( flTextDrawsLog, "TextDraw ID: %d, Text: %s\n", wTextID, cText );
	fprintf( flTextDrawsLog, "Flags: box(%i), left(%i), right(%i), center(%i), proportional(%i), padding(%i)\n", pData->BYTEBox, pData->BYTELeft, pData->BYTERight, pData->BYTECenter, pData->BYTEProportional, pData->BYTEPadding );
	fprintf( flTextDrawsLog, "LetterWidth: %.3f, LetterHeight: %.3f, LetterColor: %X, LineWidth: %.3f, LineHeight: %.3f\n", pData->fLetterWidth, pData->fLetterHeight, pData->dwLetterColor, pData->fLineWidth, pData->fLineHeight );
	fprintf( flTextDrawsLog, "BoxColor: %X, Shadow: %i, Outline: %i, BackgroundColor: %X, Style: %i, Selectable: %i\n", pData->dwBoxColor, pData->BYTEShadow, pData->BYTEOutline, pData->dwBackgroundColor, pData->BYTEStyle, pData->BYTESelectable );
	fprintf( flTextDrawsLog, "X: %.3f, Y: %.3f, ModelID: %d, RotX: %.3f, RotY: %.3f, RotZ: %.3f, Zoom: %.3f, Colors: %d, %d", pData->fX, pData->fY, pData->wModelID, pData->fRotX, pData->fRotY, pData->fRotZ, pData->fZoom, pData->wColor1, pData->wColor2 );

	if(fputs("\n\n", flTextDrawsLog) == EOF)
	{
        perror("Error writing to file");
        fclose(flTextDrawsLog);
        Globals::instance().delTXTDLog();
        return 0;
	}

	//fclose(flTextDrawsLog);
	return 1;
}

void gen_random(char *s, const int len)
{
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];

	s[len] = 0;
}

QByteArray readTextFile(const QString &file_path) {
  QFile input_file(file_path);
  QByteArray input_data;

  if (input_file.open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::ReadOnly)) {
    input_data = input_file.readAll();
    input_file.close();
    return input_data;
  }
  else
    return QByteArray();
}

QString GetPath(const char *fmt, ...) {
    if (ROOT_PATH == nullptr || strlen(ROOT_PATH) == 0)
    {
        fprintf(stderr, "ROOT_PATH is invalid\n");
        return QString();
    }

    char buffer[1024 + 1];
    memset(buffer, 0, sizeof(buffer));

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    char _PATH[1024 + 1 + 227];
    snprintf(_PATH, sizeof(_PATH), "%s/%s", ROOT_PATH, buffer);
    return QString::fromUtf8(_PATH);
}

bool isAudioPlaying(QMediaPlayer *player)
{
    if (!player)
        return false;

    return (player->playbackState() == QMediaPlayer::PlayingState &&
            (player->mediaStatus() == QMediaPlayer::LoadedMedia ||
             player->mediaStatus() == QMediaPlayer::BufferedMedia));
}

bool __setStateMessage(const char* text)
{
    if(strlen(text) < 1)
        return false;

    QSharedPointer<QLabel> _stateMsg = Globals::instance().getStateL();
    if (_stateMsg.isNull()) {
        qWarning() << "_stateMsg is not a valid pointer";
        return false;
    }
    QLabel *stateMsg = _stateMsg.get();
    if (stateMsg == nullptr || stateMsg->parent() == nullptr) {
        qWarning() << "QLabel instance is not valid.";
        return false;
    }

    char __TXT[128];
    snprintf(__TXT, sizeof(__TXT), "<p><span style=\" font-size:11pt;\">State</span>: <span style=\" font-weight:600;\">%s</span></p>", text);
    QMetaObject::invokeMethod(getMainWindow(), [stateMsg, __TXT]() {
        stateMsg->setText(QString::fromUtf8(__TXT));
    }, Qt::QueuedConnection);
    return true;
}

QStringList Char2DArrayToQStringList(char **CharTXT, int rowCount)
{
    QStringList stringList;

    for (int i = 0; i < rowCount; ++i)
    {
        if (CharTXT[i] && CharTXT[i][0] != '\0')
            stringList.append(QString::fromUtf8(CharTXT[i]));
    }
    return stringList;
}

char* formatColorMessage(const char* szMsg) {
    std::string input(szMsg);
    std::string result;

    std::regex colorRegex("\\{([A-Fa-f0-9]{6})\\}");
    std::smatch match;

    size_t lastPos = 0;

    std::string remaining = input.substr(lastPos);
    while (std::regex_search(remaining, match, colorRegex))
    {
        size_t matchPos = match.position();

        result += remaining.substr(0, matchPos);

        std::string colorCode = match[1].str();

        size_t nextPos = matchPos + match.length();
        size_t nextColorPos = remaining.find("{", nextPos);

        std::string coloredText;
        if (nextColorPos == std::string::npos)
        {
            coloredText = remaining.substr(nextPos);
            result += "<span style='color: #" + colorCode + "'>" + coloredText + "</span>";
            remaining = "";
        }
        else
        {
            coloredText = remaining.substr(nextPos, nextColorPos - nextPos);
            result += "<span style='color: #" + colorCode + "'>" + coloredText + "</span>";
            remaining = remaining.substr(nextColorPos);
        }
    }
    result += remaining;

    char *ret = new char[result.size() + 1];
    std::strcpy(ret, result.c_str());
    return ret;
}

void handleProperty(const QString &property, bool removeRow, const QString &value = QString())
{
    QSharedPointer<QStandardItemModel> _QStandardItemModel = Globals::instance().getPropertiesTree();
    QStandardItemModel *propertiesTree = _QStandardItemModel.data();

    QList<QStandardItem *> items = propertiesTree->findItems(property, Qt::MatchExactly, 0);
    if (!items.isEmpty())
    {
        int row = items.first()->row();
        if (removeRow)
        {
            propertiesTree->removeRow(row);
            return;
        }
        QStandardItem *valueItem = propertiesTree->item(row, 1);
        valueItem->setText(value);
    }
    else
    {
        if(removeRow)
            return;
        QStandardItem *propertyItem = new QStandardItem(property);
        QStandardItem *valueItem = new QStandardItem(value);
        propertyItem->setFlags(propertyItem->flags() & ~Qt::ItemIsEditable);
        valueItem->setFlags(valueItem->flags() & ~Qt::ItemIsEditable);
        propertiesTree->appendRow({propertyItem, valueItem});
    }
}

char* removeMarkupTags(char* text)
{
    std::string _text(text);
    std::regex MarkupRegex("((?=<)(?!<\\/)<)(.*?)((?= \\/>)|(?=>))");

    std::string output = std::regex_replace(_text, MarkupRegex, "");

    return const_cast<char *>(output.c_str());
}

QString removeMarkupTags(QString &text)
{
    QRegularExpression MarkupRegex("((?=<)(?!<\\/)<)(.*?)((?= \\/>)|(?=>))");
    QString output = text.replace(MarkupRegex, "");

    return output;
}

bool ToggleChatBox()
{
    QSharedPointer<QLineEdit> _messageInput = Globals::instance().getMessageInput();
    if (_messageInput.isNull()) {
        qWarning() << "_messageInput is not a valid pointer";
        return false;
    }
    QLineEdit *messageInput = _messageInput.get();
    if (messageInput == nullptr || messageInput->parent() == nullptr) {
        qWarning() << "QLineEdit instance is not valid.";
        return false;
    }
    QMetaObject::invokeMethod(getMainWindow(), [messageInput]() {
        messageInput->setEnabled(!messageInput->isEnabled());
    }, Qt::QueuedConnection);
    return true;
}

bool ToggleSendButton()
{
    QSharedPointer<QPushButton> _sendButton = Globals::instance().getSendButton();
    if (_sendButton.isNull()) {
        qWarning() << "_sendButton is not a valid pointer";
        return false;
    }
    QPushButton *sendButton = _sendButton.get();
    if (sendButton == nullptr || sendButton->parent() == nullptr)
    {
        qWarning() << "QPushButton instance is not valid.";
        return false;
    }
    QMetaObject::invokeMethod(getMainWindow(), [sendButton]() {
        sendButton->setEnabled(!sendButton->isEnabled());
    }, Qt::QueuedConnection);
    return true;
}

bool startsWith(const char* text, const char* prefix)
{
    if(text == nullptr || prefix == nullptr)
        return false;

    size_t textLen = strlen(text);
    size_t prefixLen = strlen(prefix);

    if(prefixLen > textLen)
        return false;

    return (strncmp(text, prefix, prefixLen) == 0);
}

// Thanks to 81Vm3 for RakMagic
const char *getWeaponName(int weaponid) {
	return (weapon_list + weaponid)->name;
}

bool IsVehicleIndexValid(int vehicleid) {
	if (vehicleid >= MAX_VEHICLES || vehicleid < 1)
		return false;
    if (!vehiclePool[vehicleid].iExist)
        return false;
	return true;
}

void centerOverParent(QWidget* child, QWidget* parent) {
    if (!child || !parent) return;
    QScreen *screen = QGuiApplication::screenAt(parent->geometry().center());
    if (!screen)
        screen = QGuiApplication::primaryScreen();
    QRect parentScreenGeom = screen->geometry();
    QPoint center = parent->geometry().center();
    QPoint childPos = center - QPoint(child->width() / 2, child->height() / 2);

    child->move(childPos);
}
