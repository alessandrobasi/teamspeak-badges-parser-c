#include "TeamspeakBadgesViewer.h"
#include <QDir>
#include <QTimer>
#include <QRegularExpression>
#include <qapplication.h>
#include "DownloadManager.h"
#include <QMessageBox>
#include "InfoDialog.h"

TeamspeakBadgesViewer::TeamspeakBadgesViewer(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    this->show(); // show gui
    ui.statusMessage->setText("Loading...");

    

    this->getFile(); // download file "list"
    
    if (!QFile("list").exists()) {
        QMessageBox messageBox;
        messageBox.critical(parent, "Error", "File 'list' missing");
        messageBox.setFixedSize(500, 200);
        return;
    }
    
    // main Window
    connect(ui.menuInfo, &QMenu::aboutToShow, this, &TeamspeakBadgesViewer::openInfo);

    // set QTableWidget rows
    int numBadges = this->badgesInfo.length();
    ui.tabellaBages->setRowCount(numBadges);

    // connect button and slection
    connect(ui.clearCacheBtn, &QPushButton::clicked, this, &TeamspeakBadgesViewer::clearCache);
    connect(ui.tabellaBages, &QTableWidget::itemSelectionChanged, this, &TeamspeakBadgesViewer::showBadgeInfo);

    int index = 0;
    for each (auto badge in this->badgesInfo) {
        QIcon icon_t(this->getBadgeIcon(badge.at(0), badge.at(2))); // get(and download) badge icon
        QTableWidgetItem* item = new QTableWidgetItem(icon_t, badge.at(1)); // create item for table
        item->setData(Qt::UserRole, QVariant(badge)); // add data to item
        ui.tabellaBages->setItem(index++, 0, item); // insert in table
    }
    // set message to statusbar
    QString _statusMsg = QString::number(numBadges) + " Badges";
    ui.statusMessage->setText(_statusMsg);
}

void TeamspeakBadgesViewer::openInfo() {
    qDebug("Dialog open");
    InfoDialog about;
    about.exec();
}

void TeamspeakBadgesViewer::getFile() {

    // inizialize file and dir for badges
    QDir cache("cache");
    QFile rawBadges("list");

    // create cache dir
    if (!cache.exists()) {
        if (!cache.mkpath("cache")) {
            qDebug("Error cache dir creation");
            return;
        }
    }
    
    // get system time and modified time of the file "list"
    QDateTime now = QDateTime::currentDateTime();
    QDateTime fileTime = rawBadges.fileTime(QFileDevice::FileModificationTime);

    // True if 
    //      (file don't exist) OR (file empty) OR (file old than 7 days)
    bool download = !rawBadges.exists() || rawBadges.size() == 0 || fileTime.secsTo(now) > 604800;

    qDebug("sysTime: %s fileTime: %s diff: %s", 
        qPrintable(now.toString()), 
        qPrintable(fileTime.toString()), 
        qPrintable(QString::number(fileTime.secsTo(now)))
    );
    qDebug("downloading? %d ", download);

    // if file must be downloaded
    if (download) {
        qDebug("Download badge list");

        if (!rawBadges.open(QIODevice::WriteOnly)) {
            qDebug("Error open file (Write)");
            return;
        }

        QByteArray result = this->_timeoutLoop(this->urlBadges);

        if (result.size() > 0) {
            rawBadges.write(result);
        }
        
        rawBadges.close();
    }

    // il file "list" è presente, leggi il file e crea una struttura
    if (!rawBadges.open(QIODevice::ReadOnly)) {
        qDebug("Error open file (Read)");
        return;
    }

    

    QString dataBadges = QString::fromStdString(rawBadges.readAll().toStdString());
    
    rawBadges.close();

    QRegularExpression expr("^\\$(?P<headid>[\\w\\d-]+).(.|\\n)(?P<nome>.+)..(?P<url>https://[\\w\\-\\.\\/]+)..(?P<desc>\\w.*)\\(.+$",
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
    );

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

QString TeamspeakBadgesViewer::getBadgeIcon(QString uuid, QString url, QString type) {
    QFile icon("cache/" + uuid + type);
    if (!icon.exists() || icon.size() == 0) {

        if (!icon.open(QIODevice::WriteOnly)) {
            qDebug("Error open icon (Write)");
            return "";
        }

        QByteArray result = this->_timeoutLoop(url + type);

        if (result.size() > 0) {
            icon.write(result); // scrivo su file
        }
        icon.close();
    }

    return "cache/" + uuid + type;
}

void TeamspeakBadgesViewer::showBadgeInfo() {

    QTableWidgetItem* item = ui.tabellaBages->selectedItems().at(0);
    QVariantList r_badge = item->data(Qt::UserRole).toList();

    QString uuid = r_badge.at(0).toString();
    QString nome = r_badge.at(1).toString();
    QString url  = r_badge.at(2).toString();
    QString desc = r_badge.at(3).toString();

    QPixmap icon(this->getBadgeIcon(uuid, url));
    ui.iconaBadge->setPixmap(icon);
    ui.nomeBadge->setText(nome);
    ui.descBadge->setText(desc);
    ui.uuidText->setText(uuid);

    ui.svgUrl->setText("SVG: <a href='" + url + ".svg" + "'><span style='text-decoration:none;color:#3c76cc;'>[...].svg" + "</span></a>");
    ui.detailsSvgUrl->setText("Details SVG: <a href='" + url + "_details.svg" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_details.svg" + "</span></a>");
    ui.pngUrl->setText("PNG: <a href='" + url +
        "_16.png" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_16.png" + "</span></a>");
    ui.detailsPngUrl->setText("Details PNG: <a href='" + url + "_64.png" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_64.png" + "</span></a>");

}

void TeamspeakBadgesViewer::clearCache() {
    
    QDir cache("cache");
    QFile list("list");

    if (!cache.removeRecursively()) {
        qDebug("Error cleaning cache dir");
    }

    if (!list.remove()) {
        qDebug("Error removing list file");
    }

    QApplication::quit();
}

//TODO: move to thread
QByteArray TeamspeakBadgesViewer::_timeoutLoop(QString fileToDownload, int time) {

    // creo un loop per dare tempo al network di scaricare il file
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit); // avviene in caso di timeout del network

    DownloadManager downloadFile(fileToDownload, this); // avvio il download
    connect(&downloadFile, SIGNAL(downloaded()), &loop, SLOT(quit())); // avviene quando il file viene scaricato 

    timeout.start(time);
    loop.exec(); // loop infito

    if (timeout.isActive()) {
        timeout.stop();
        return downloadFile.downloadedData(); // scrivo su file
    }
    else {
        qDebug("Error downloading file (timeout)");
        return QByteArray();
    }
    
}
