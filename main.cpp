#include "src/TeamspeakBadgesViewer.h"
#include <QtWidgets/QApplication>
#include <QtNetwork/qsslsocket.h>
#include <QMessageBox>
#include "src/sqlite/Sql.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug() << QSslSocket::sslLibraryBuildVersionString();
    qDebug() << QSslSocket::supportsSsl();
    qDebug() << QSslSocket::sslLibraryVersionString();

    // check if ssl dll is missing
    if (!QSslSocket::supportsSsl()) {
        QMessageBox messageBox(
            QMessageBox::Critical, 
            "Missing Dll", 
            "Missing some dll,\ninstall from https://www.microsoft.com/en-us/download/details.aspx?id=13523",
            QMessageBox::Ok
        );
        messageBox.setFixedSize(500, 200);
        messageBox.exec();
        return 1;
    }
    
    QStringList _t = QSqlDatabase::drivers();
    qDebug() << _t;

    //  check if SQL driver is missing
    if (!_t.contains("QSQLITE")) {
        QMessageBox messageBox(
            QMessageBox::Critical,
            "Missing QSQLITE driver",
            "Missing QSQLITE driver,\nThis should not appear",
            QMessageBox::Ok
        );
        messageBox.setFixedSize(500, 200);
        messageBox.exec();
        return 1;
    }

    // start app
    TeamspeakBadgesViewer w;
    return a.exec();
}
