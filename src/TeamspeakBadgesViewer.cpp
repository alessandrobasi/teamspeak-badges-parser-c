#include "TeamspeakBadgesViewer.h"
#include <QRegularExpression>
#include <QPixmap>


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
    connect(ui.badgesTable, &QTableWidget::itemSelectionChanged, this, &TeamspeakBadgesViewer::showBadgeInfo);
    
}

// open dialog box for show info
void TeamspeakBadgesViewer::openInfo() {
    qDebug("Dialog open");
    InfoDialog about;
    about.exec();
}

// download rawlist badges
void TeamspeakBadgesViewer::getFile() {

    QString lastEdit = QString();
    QString datetime_en = QString();

    QSqlQuery SqlRawList;

    // get saved (if present) rawlist and lastedit
    this->storeBadges->getRawList(SqlRawList);

    // if data in database fill variable
    if (SqlRawList.first()) {
        lastEdit = SqlRawList.value("lastedit").toString();
        this->rawList = SqlRawList.value("rawlist").toString();
    }

    // transform string to datetime data
    QDateTime fileTime = QDateTime::fromString(lastEdit, "yyyy-MM-dd hh:mm:ss");

    if (fileTime.isValid()) {
        // set tranlation for name of the day/month
        datetime_en = QLocale(QLocale::English).toString(fileTime, "ddd, dd MMM yyyy hh:mm:ss") + " GMT";
    }

    // default for datetime_en is "Mon, 01 Jan 1990 00:00:00 GMT"
    HttpHead* downloadFile = new HttpHead(this->urlBadges, 3000, datetime_en);

    qDebug("LastEdit badges list: %s", qPrintable(datetime_en));

    // event at complete download
    connect(downloadFile, &HttpHead::downloaded, this, &TeamspeakBadgesViewer::getDownloadedList);

    // start download rawList
    downloadFile->start();
}

// extract info from list 
void TeamspeakBadgesViewer::getDownloadedList(QByteArray result) {
    // at rawList download

    if (result.size() == 0) { // Error
        qDebug() << "Error downloading";
        ui.statusMessage->setText("Error Downloading");
        return;
    }
    else if (result.size() == 1) { // Badge list not modified (so icon too)
        qDebug() << "not modified";
        // use saved info in db
    }
    else if (result.size() > 1) { // parse and get new icon
        this->storeBadges->addRawList(result);
        this->rawList = result.toStdString().c_str();

        // regex magic
        QRegularExpression expr(this->regExStr,
            QRegularExpression::CaseInsensitiveOption | QRegularExpression::MultilineOption
        );
        // generate iterator through RegEx groups
        QRegularExpressionMatchIterator i = expr.globalMatch(this->rawList);
        int count = 0;
        // save info in db
        while (i.hasNext()) {

            // get next RegEx group
            QRegularExpressionMatch match = i.next();
            
            // store to db
            if (!storeBadges->addBadge(
                match.captured("guid"),
                match.captured("nome"),
                match.captured("url"),
                match.captured("desc")
            )) {

                qDebug() << "Error saving badge id " << match.captured("guid");
                return;
            }

        }

    }

    // get result query
    QSqlQuery Sqlbadges;
    this->storeBadges->getBadges(Sqlbadges);

    /*
    0 id
    1 guid
    2 name
    3 url
    4 desc
    */

    int x = 0;
    // populate
    while (Sqlbadges.next()) {
        // set number of row for table
        ui.badgesTable->setRowCount(x+1);

        // create item for table
        QTableWidgetItem* item = new QTableWidgetItem(Sqlbadges.value("name").toString());

        // add data to item
        item->setData(Qt::UserRole, Sqlbadges.value("guid"));


        QSqlQuery badgeData;
        this->storeBadges->getGuidData(Sqlbadges.value("guid").toString(), badgeData);

        /*
        0 guid
        1 _64png
        2 lastedit
        */

        if (badgeData.next()) {
            QPixmap _pixmap;
            _pixmap.loadFromData(badgeData.value("_64png").toByteArray());

            item->setIcon(QIcon(_pixmap));
        }
        else {
            // TODO: istanziare i puntatori di downloadIcon in una lista???
            HttpHead* downloadIcon = new HttpHead(Sqlbadges.value("url").toString() + "_64.png", 3000);
            connect(downloadIcon, &HttpHead::downloaded, this, [=](QByteArray resultData) {
                this->getDownloadedBadgeIcon(item, resultData);
                });
            downloadIcon->start();
        }

        badgeData.clear();

        // insert in table
        ui.badgesTable->setItem(x, 0, item);
        x++;
    }

    // set message to statusbar
    ui.statusMessage->setText(QString::number(x) + " Badges");
}

// on click badges
void TeamspeakBadgesViewer::showBadgeInfo() {
    QString url, desc;

    QSqlQuery info;

    QTableWidgetItem* item = ui.badgesTable->selectedItems().at(0);
    
    QString guid = item->data(Qt::UserRole).toString();
    QString name = item->text();
    QPixmap icon = item->icon().pixmap(64, 64);

    this->storeBadges->getInfo(guid, info);

    if (info.first()) {
         url  = info.value("url").toString();
         desc = info.value("desc").toString();
    }

    ui.iconaBadge->setPixmap(icon);
    ui.nomeBadge->setText(name);

    ui.descBadge->setText(desc);
    ui.uuidText->setText(guid);

    ui.svgUrl->setText("SVG: <a href='" + url + ".svg" + "'><span style='text-decoration:none;color:#3c76cc;'>[...].svg" + "</span></a>");
    ui.detailsSvgUrl->setText("Details SVG: <a href='" + url + "_details.svg" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_details.svg" + "</span></a>");
    ui.pngUrl->setText("PNG: <a href='" + url + "_16.png" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_16.png" + "</span></a>");
    ui.detailsPngUrl->setText("Details PNG: <a href='" + url + "_64.png" + "'><span style='text-decoration:none;color:#3c76cc;'>[...]_64.png" + "</span></a>");

}

// clear all content
void TeamspeakBadgesViewer::clearCache() {

    QFile dbstore(this->databaseName);

    if (!dbstore.remove()) {
        qDebug("Error removing list file");
    }

    qApp->quit();
}

// on downloaded icon
void TeamspeakBadgesViewer::getDownloadedBadgeIcon(QTableWidgetItem* item, QByteArray rawIcon) {
    
    qDebug() << item->text() << " " << rawIcon.size();


    if (rawIcon.size() == 0) { // Error
        qDebug() << "Error downloading";
        ui.statusMessage->setText("Error Downloading icon");
        return;
    }
    else if (rawIcon.size() == 1) { // Icon not modified 
        qDebug() << "not modified";
    }
    else if (rawIcon.size() > 1) { // get new icon
        
        // save to db
        if (!this->storeBadges->addIcon(item->data(Qt::UserRole).toString(), rawIcon)) {
            qDebug() << "Error saving icon for badge: " << item->data(Qt::UserRole).toString();
            return;
        }

        // set icon
        QPixmap _pixmap;
        _pixmap.loadFromData(rawIcon);

        item->setIcon(QIcon(_pixmap));

    }
}
