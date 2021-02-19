#include "src/TeamspeakBadgesViewer.h"
#include <QtWidgets/QApplication>
#include <QtNetwork/qsslsocket.h>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << QSslSocket::sslLibraryBuildVersionString();

    qDebug() << QSslSocket::supportsSsl();

    qDebug() << QSslSocket::sslLibraryVersionString();

    if (!QSslSocket::supportsSsl()) {
        QMessageBox messageBox;
        messageBox.critical(0, "Error", "Missing some dll, install from <a>https://www.microsoft.com/en-us/download/details.aspx?id=13523</a>");
        messageBox.setFixedSize(500, 200);
        return 0;
    }

    TeamspeakBadgesViewer w;
    //w.show();
    return a.exec();
}
