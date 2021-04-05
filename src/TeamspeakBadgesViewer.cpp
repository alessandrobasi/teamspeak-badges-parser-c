#include "TeamspeakBadgesViewer.h"
#include <QTimer>
#include <QRegularExpression>
#include <qapplication.h>
#include "DownloadManager.h"
#include <QMessageBox>
#include "InfoDialog.h"
#include "HttpHead.h"

TeamspeakBadgesViewer::TeamspeakBadgesViewer(QWidget *parent)
    : QMainWindow(parent)
{
    // load gui first
    ui.setupUi(this);
    this->show(); // show gui
    ui.statusMessage->setText("Loading...");

    this->getFile(); // download file "list"
    
    // main Window
    connect(ui.menuInfo, &QMenu::aboutToShow, this, &TeamspeakBadgesViewer::openInfo);

    // connect button and slection
    connect(ui.clearCacheBtn, &QPushButton::clicked, this, &TeamspeakBadgesViewer::clearCache);
    connect(ui.tabellaBages, &QTableWidget::itemSelectionChanged, this, &TeamspeakBadgesViewer::showBadgeInfo);

}

void TeamspeakBadgesViewer::openInfo() {
    qDebug("Dialog open");
    InfoDialog about;
    about.exec();
}

void TeamspeakBadgesViewer::getDownloadedList(QByteArray result) {

    // check if file has data
    if (result.size() > 0) {
        if (!this->rawBadges->open(QIODevice::WriteOnly)) {
            qDebug("Error open file (Write)");
            return;
        }

        this->rawBadges->write(result);

        this->rawBadges->close();
    }
    else {
        qDebug() << "0 data recived" ;
    }

    // open badges file and read it 
    if (!this->rawBadges->open(QIODevice::ReadOnly)) {
        qDebug("Error open file (Read)");
        return;
    }

    QString dataBadges = QString::fromStdString(this->rawBadges->readAll().toStdString());

    this->rawBadges->close();


    // regex magic
    QRegularExpression expr(this->regExStr,
        QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
    );
    // generate iterator through RegEx groups
    QRegularExpressionMatchIterator i = expr.globalMatch(dataBadges);


    // initial table rows
    int num_row = 0;

    // create information list
    while (i.hasNext()) {
        // set new row count
        num_row++;
        ui.tabellaBages->setRowCount(num_row);

        // get next RegEx group
        QRegularExpressionMatch match = i.next();

        // TODO: ottimizzare
        // build list for TableWidgetItem Data
        QList<QString> temp;
        temp << match.captured("headid");
        temp << match.captured("nome");
        temp << match.captured("url");
        temp << match.captured("desc");


        // get(and download) badge icon
        QIcon icon_t(this->getBadgeIcon(match.captured("headid"), match.captured("url")));
        // create item for table
        QTableWidgetItem* item = new QTableWidgetItem(icon_t, match.captured("nome"));
        // add data to item
        item->setData(Qt::UserRole, QVariant(temp));
        // insert in table
        ui.tabellaBages->setItem(num_row, 0, item); 

    }

    // set message to statusbar
    QString _statusMsg = QString::number(num_row) + " Badges";
    ui.statusMessage->setText(_statusMsg);
    
}

void TeamspeakBadgesViewer::getFile() {

    // inizialize file and dir for badges
    
    // create cache dir
    if (!this->cache->exists()) {
        if (!this->cache->mkpath(".")) {
            qDebug("Error cache dir creation");
            return;
        }
    }
    

    // get system time and modified time of the file "list"
    QDateTime fileTime = this->rawBadges->fileTime(QFileDevice::FileModificationTime).toUTC();

    QLocale locale = QLocale(QLocale::English);
    QString datetime_en = "";

    HttpHead* downloadFile = NULL;

    if (!fileTime.isNull()) {
        datetime_en = locale.toString(fileTime, "ddd, dd MMM yyyy hh:mm:ss") + " GMT";
    }
    downloadFile = new HttpHead(this->urlBadges, 3000, datetime_en);
    

    qDebug("fileTime: %s", qPrintable(datetime_en) );
    qDebug("Try to download badge list");

    // event at complete download
    connect(downloadFile, &HttpHead::downloaded, this, &TeamspeakBadgesViewer::getDownloadedList);
     
    // start download list file
    downloadFile->start();

    // end
}

QString TeamspeakBadgesViewer::getBadgeIcon(QString uuid, QString url, QString type) {
    QFile icon("cache/" + uuid + type);
    if (!icon.exists() || icon.size() == 0) {

        QByteArray result = this->_timeoutLoop(url + type, icon.fileTime(QFileDevice::FileModificationTime));

        if (result.size() > 0) {
            if (!icon.open(QIODevice::WriteOnly)) {
                qDebug("Error open icon (Write)");
                return "";
            }

            icon.write(result); // scrivo su file

            icon.close();
        }
        
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
QByteArray TeamspeakBadgesViewer::_timeoutLoop(QUrl fileToDownload, QDateTime modifiedTime, int time) {

    // creo un loop per dare tempo al network di scaricare il file
    QEventLoop loop;
    QTimer timeout;
    timeout.setSingleShot(true);
    connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit); // avviene in caso di timeout del network

    //HttpHead downloadFile(fileToDownload, "Mon, 01 Jan 1990 00:00:00 GMT", time); // avvio il download
    //connect(&downloadFile, SIGNAL(downloaded()), &loop, SLOT(quit())); // avviene quando il file viene scaricato 

    timeout.start(time);
    loop.exec(); // loop infito

    if (timeout.isActive()) {
        timeout.stop();
        //return downloadFile.downloadedData(); // scrivo su file
    }
    else {
        qDebug("Error downloading file (timeout)");
        return QByteArray();
    }
    
}
