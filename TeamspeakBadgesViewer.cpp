#include "TeamspeakBadgesViewer.h"
#include <QDir>
#include <QTimer>
#include <QRegularExpression>
#include "DownloadManager.h"

TeamspeakBadgesViewer::TeamspeakBadgesViewer(QWidget *parent)
    : QMainWindow(parent)
{
    this->getFile();
    ui.setupUi(this);

    // main Window
    int numBadges = this->badgesInfo.length();
    QString _statusMsg = QString::number(numBadges) + " Medaglie";
    ui.statusBar->showMessage(_statusMsg);
    ui.tabellaBages->setRowCount(numBadges);
    connect(ui.clearCacheBtn, &QPushButton::clicked, this, &TeamspeakBadgesViewer::clearCache);
    int index = 0;
    for each (auto badge in this->badgesInfo) {

        QTableWidgetItem item(this->getBadgeIcon(badge.at(0), badge.at(2)), badge.at(1));
        item.setData(Qt::UserRole, QVariant(badge));
        ui.tabellaBages->setItem(index++, 0, &item);
    }

}

void TeamspeakBadgesViewer::getFile() {

    QDir cache("cache");
    QFile rawBadges("list");

    // creo se non c'è la cartella cache
    if (!cache.exists()) {
        if (!cache.mkpath("cache")) {
            qDebug("Error cache dir creation");
            return;
        }
    }
    
    // ottengo il tempo attuale e il tempo di modifica del file "list"
    QDateTime now = QDateTime::currentDateTime();
    QDateTime fileT = rawBadges.fileTime(QFileDevice::FileModificationTime);

    qDebug("sysT: %s fileT: %s", qPrintable(now.toString()), qPrintable(fileT.toString()));
    qDebug(" %d |||||  1 = downloading , 0 = not downloading", now.secsTo(fileT) > 604800 );
    // controllo se il file non esiste o è vecchio di 7 giorni
    if (!rawBadges.exists() || now.secsTo(fileT) > 604800) {
        qDebug("Download badge list");

        if (!rawBadges.open(QIODevice::WriteOnly)) {
            qDebug("Error open file (Write)");
            return;
        }

        // creo un loop per dare tempo al network di scaricare il file
        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit); // avviene in caso di timeout del network

        DownloadManager downloadFile(this->urlBadges, this); // avvio il download
        connect(&downloadFile, SIGNAL(downloaded()), &loop, SLOT(quit())); // avviene quando il file viene scaricato 

        timeout.start(2500);
        loop.exec(); // loop infito

        if (timeout.isActive()) {
            timeout.stop();
            rawBadges.write(downloadFile.downloadedData()); // scrivo su file
        }
        else {
            qDebug("Error downloading file (timeout)");
        }
        rawBadges.close();
    }

    // il file "list" è presente, leggi il file e crea una struttura
    if (!rawBadges.open(QIODevice::ReadOnly)) {
        qDebug("Error open file (Read)");
        return;
    }

    QRegularExpression expr("^\\$(?P<headid>[\\w\\d-]+).(.|\\n)(?P<nome>.+)..(?P<url>https://[\\w\\-\\.\\/]+)..(?P<desc>\\w.*)\\(.+$",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption );

    QString dataBadges = QString::fromStdString(rawBadges.readAll().toStdString());
        
    QRegularExpressionMatchIterator i = expr.globalMatch(dataBadges);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QList<QString> temp;
        temp << match.captured("headid");
        temp << match.captured("nome");
        temp << match.captured("url");
        temp << match.captured("desc");
        this->badgesInfo << temp;
    }

    

}

QIcon TeamspeakBadgesViewer::getBadgeIcon(QString uuid, QString url, QString type) {
    QFile icon("cache/" + uuid + type);
    if (!icon.exists()) {

        if (!icon.open(QIODevice::WriteOnly)) {
            qDebug("Error open icon (Write)");
            return QIcon();
        }

        QEventLoop loop;
        QTimer timeout;
        timeout.setSingleShot(true);
        connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit); // avviene in caso di timeout del network

        DownloadManager downloadFile(url+type, this); // avvio il download
        connect(&downloadFile, SIGNAL(downloaded()), &loop, SLOT(quit())); // avviene quando il file viene scaricato 

        timeout.start(2500);
        loop.exec(); // loop infito

        if (timeout.isActive()) {
            timeout.stop();
            icon.write(downloadFile.downloadedData()); // scrivo su file
        }
        else {
            qDebug("Error downloading file (timeout)");
        }
        icon.close();
    }

    return QIcon("cache/" + uuid + type);
}

void TeamspeakBadgesViewer::clearCache() {
    qDebug("clicked");
}
