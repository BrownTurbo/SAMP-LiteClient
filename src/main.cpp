#include "main.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qRegisterMetaType<QTextCursor>("QTextCursor");
    QString styleSheet = readTextFile(GetPath("stylesheets/app.qss"));
    styleSheet.replace("{ROOT_PATH}", ROOT_PATH);
    app.setStyleSheet(styleSheet);

    QString windowIconP = GetPath("resources/img/logo.ico");
    app.setWindowIcon(QIcon(windowIconP));

    MainWindow mainWindow;
    mainWindow.show();
    QScreen* screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->geometry();
        mainWindow.move(screenGeometry.center() - mainWindow.rect().center());
    }
    return app.exec();
}
