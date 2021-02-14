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
    void getBadgeIcon();
    void showBadgeInfo();
    void clearCache();
    void changeScreen();
};
