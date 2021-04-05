#pragma once

#include <QtWidgets/QMainWindow>
#include <QUrl>
#include <QDir>
#include <QFile>
#include "ui_TeamspeakBadgesViewer.h"

class TeamspeakBadgesViewer : public QMainWindow
{
    Q_OBJECT

public:
    TeamspeakBadgesViewer(QWidget *parent = Q_NULLPTR);

public slots:
    void openInfo();
    void getDownloadedList(QByteArray result);

private:
    Ui::TeamspeakBadgesViewerClass ui;
    QUrl urlBadges = QUrl("https://badges-content.teamspeak.com/list");
    QList<QList<QString>> badgesInfo;
    QDir* cache = new QDir("cache");
    QFile* rawBadges = new QFile("list");
    QString regExStr = "^\\$(?P<headid>[\\w\\d-]+).(.|\\n)(?P<nome>.+)..(?P<url>https://[\\w\\-\\.\\/]+)..(?P<desc>\\w.*)\\(.+$";


    void getFile();
    QString getBadgeIcon(QString uuid, QString url, QString type = "_64.png");
    void showBadgeInfo();
    void clearCache();
    QByteArray _timeoutLoop(QUrl fileToDownload, QDateTime modifiedTime, int time = 3000);
};
