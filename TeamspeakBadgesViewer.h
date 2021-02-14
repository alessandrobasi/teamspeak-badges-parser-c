#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TeamspeakBadgesViewer.h"

class TeamspeakBadgesViewer : public QMainWindow
{
    Q_OBJECT

public:
    TeamspeakBadgesViewer(QWidget *parent = Q_NULLPTR);

private:
    Ui::TeamspeakBadgesViewerClass ui;
    const QString urlBadges = "https://badges-content.teamspeak.com/list";
    QList<QList<QString>> badgesInfo;
    void getFile();
    QIcon getBadgeIcon(QString uuid, QString url, QString type = "_64.png");
    void showBadgeInfo();
    void clearCache();
    void changeScreen();
};
