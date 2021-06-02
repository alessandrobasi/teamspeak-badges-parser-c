#pragma once

#include <QtWidgets/QMainWindow>
#include <QUrl>
#include "ui_TeamspeakBadgesViewer.h"
#include "InfoDialog.h"
#include "HttpHead.h"
#include "sqlite/Sql.h"

class TeamspeakBadgesViewer : public QMainWindow
{
    Q_OBJECT

public:
    TeamspeakBadgesViewer(QWidget *parent = Q_NULLPTR);
    

public slots:
    void openInfo();
    void getDownloadedList(QByteArray result);
    void getDownloadedBadgeIcon(QTableWidgetItem* item, QByteArray rawIcon);

private:
    Ui::TeamspeakBadgesViewerClass ui;
    QString databaseName = "badgesData.db";
    Sql *storeBadges = new Sql(databaseName);
    QString rawList = QString();

    QUrl urlBadges = QUrl("https://badges-content.teamspeak.com/list");
    
    // https://regex101.com/r/rnyijt/1
    QString regExStr = "^\\$(?P<guid>[\\w\\d-]+).(.|\\n)(?P<nome>.+)..(?P<url>https://[\\w\\-\\.\\/]+)..(?P<desc>\\w.*)\\(.+$";


    void getFile();
    void showBadgeInfo();
    void clearCache();
};
