#include "Sql.h"

Sql::Sql(QString dbname) {

	QFileInfo dbFileInfo(dbname);

	// automatically create file if not exists

	this->db = QSqlDatabase::addDatabase("QSQLITE");
	this->db.setDatabaseName(dbFileInfo.absoluteFilePath());
	this->db.open();
	
	if (!this->db.open()) {
		qDebug() << "Error: connection with database fail";
		return;
	}
	else {
		qDebug() << "Database: connection ok";
	}

	// create sql schema
	QSqlQuery _query(this->db);
	this->query = _query;
	// schema for rawList
	this->query.exec("CREATE TABLE IF NOT EXISTS `tsbadges_list` ("
		"`id`	INTEGER NOT NULL UNIQUE,"
		"`lastedit`	TEXT DEFAULT CURRENT_TIMESTAMP,"
		"`rawlist`	BLOB"
		");"
	);
	// schema for badges info
	this->query.exec("CREATE TABLE IF NOT EXISTS `tsbadges_info` ("
		"`id`	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,"
		"`guid`	TEXT NOT NULL UNIQUE,"
		"`name`	TEXT NOT NULL,"
		"`url`	TEXT NOT NULL,"
		"`desc`	TEXT NOT NULL,"
		"`lastedit`	TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP"
		");"
	);
	// schema for badges icon
	this->query.exec("CREATE TABLE IF NOT EXISTS `tsbadges_data` ("
		"`guid`	TEXT NOT NULL UNIQUE,"
		"`_64png`	BLOB,"
		"`lastedit`	TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,"
		"FOREIGN KEY(`guid`) REFERENCES `tsbadgesinfo`(`guid`),"
		"PRIMARY KEY(`guid`)"
		");"
	);

	// create indexes
	this->query.exec(
		"CREATE UNIQUE INDEX IF NOT EXISTS `guid` ON `tsbadges_info` (`id`);"
	);
	this->query.exec(
		"CREATE UNIQUE INDEX IF NOT EXISTS `guid_data` ON `tsbadges_data` (`guid`);"
	);
}

void Sql::close() {
	this->db.close();
}

bool Sql::addBadge(QString guid, QString name, QString url, QString desc) {

	if (this->db.transaction()) {

		this->query.prepare("INSERT INTO `tsbadges_info` "
			"(guid, name, url, desc) VALUES " 
				"(:guid, :name, :url, :desc) "
			"ON CONFLICT(guid) DO "
				"UPDATE SET "
					"name = :name, "
					"desc = :desc, "
					"lastedit = CURRENT_TIMESTAMP "
			"WHERE guid = :guid "
			";"
		);
		this->query.bindValue(":guid", guid);
		this->query.bindValue(":name", name);
		this->query.bindValue(":url" , url);
		this->query.bindValue(":desc", desc);
		
		if (!this->query.exec()) {
			qDebug() << "Failed query addBadge for " << guid;
			this->db.rollback();
			return false;
		}

		if (!this->db.commit()) {
			qDebug() << "Failed to commit query addBadge for " << guid;
			this->db.rollback();
			return false;
		}

		return true;
	}
	else {
		qDebug() << "Failed to start transaction mode";
		return false;
	}
	return false;
}

bool Sql::addIcon(QString guid, QByteArray _64png) {
	if (this->db.transaction()) {

		this->query.prepare("INSERT INTO "
			"`tsbadges_data` (guid, _64png, lastedit) VALUES "
			"(:guid, :_64png, CURRENT_TIMESTAMP)"
			"ON CONFLICT(guid) DO "
				"UPDATE SET "
				"_64png = :_64png, "
				"lastedit = CURRENT_TIMESTAMP "
			"WHERE guid = :guid "
			";"
		);

		this->query.bindValue(":guid", guid);
		this->query.bindValue(":_64png", _64png);
		
		if (!this->query.exec()) {
			qDebug() << "Failed query addIcon for " << guid;
			this->db.rollback();
			return false;
		}

		if (!this->db.commit()) {
			qDebug() << "Failed to commit query addIcon for " << guid;
			this->db.rollback();
			return false;
		}
		return true;
	}
	else {
		qDebug() << "Failed to start transaction mode";
		return false;
	}
	return false;
}

void Sql::addRawList(QByteArray rawList) {

	if (this->db.transaction()) {

		this->query.prepare("REPLACE INTO "
			"`tsbadges_list` (id, rawlist) VALUES "
			"(1, :rawlist)"
			";"
		);

		this->query.bindValue(":rawlist", rawList);
		this->query.exec();

		if (!this->db.commit()) {
			qDebug() << "Failed to commit query addRawList";
			this->db.rollback();
		}
	}
	else {
		qDebug() << "Failed to start transaction mode";
	}

}

void Sql::getRawList(QSqlQuery& result) {

	if (this->query.exec("SELECT lastedit,rawlist FROM `tsbadges_list` WHERE ID=1;")) {
		result = this->query;
	}
}

void Sql::getBadges(QSqlQuery &result) {

	if (this->query.exec("SELECT "
		"id,guid,name,url,desc "
		"FROM tsbadges_info "
		"ORDER BY id DESC;"
	)) {
		result = this->query;
	}

}

void Sql::getGuidData(QString guid, QSqlQuery& result) {
	
	this->query.prepare("SELECT "
		"guid,_64png,lastedit "
		"FROM tsbadges_data "
		"WHERE guid = :guid ;"
	);
	this->query.bindValue(":guid", guid);
	
	if (this->query.exec()) {
		result = this->query;
	}
}

void Sql::getInfo(QString guid, QSqlQuery &result) {
	this->query.prepare("select "
		"url,desc "
		"from tsbadges_info "
		"where guid = :guid;"
	);
	this->query.bindValue(":guid", guid);

	if (this->query.exec()) {
		result = this->query;
	}
}