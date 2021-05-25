#pragma once

#include <QtWidgets/QMainWindow>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QtConcurrent>
#include "ui_TeamspeakBadgesViewer.h"
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
    
    typedef struct _BADGE {
        int id;
        QString guid;
        QString name;
        QString url;
        QString desc;
    } badge;
    QList<badge> badges;
    
    
    QString regExStr = "^\\$(?P<guid>[\\w\\d-]+).(.|\\n)(?P<nome>.+)..(?P<url>https://[\\w\\-\\.\\/]+)..(?P<desc>\\w.*)\\(.+$";


    void getFile();
    
    void showBadgeInfo();
    void clearCache();
    QByteArray _timeoutLoop(QUrl fileToDownload, QDateTime modifiedTime, int time = 3000);
};
