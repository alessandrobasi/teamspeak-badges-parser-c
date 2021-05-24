#pragma once
#include <QtSql>
#include <QFileInfo>

class Sql {

public:
	Sql(QString dbname);
	bool addBadge(QString guid, QString name, QString url, QString desc);
	bool addIcon(QString guid, QByteArray _64png);
	void addRawList(QByteArray rawList);
	
	void getRawList(QSqlQuery& result);
	void getBadges(QSqlQuery& result);
	void getGuidData(QString guid, QSqlQuery& result);
	void getInfo(QString guid, QSqlQuery& result);

private:
	QSqlDatabase db;
	QSqlQuery query;
};

