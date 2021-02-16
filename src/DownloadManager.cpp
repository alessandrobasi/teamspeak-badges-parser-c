#include "DownloadManager.h"

DownloadManager::DownloadManager(QUrl url, QObject* parent) :
	QObject(parent) {
	connect(
		&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(fileDownloaded(QNetworkReply*))
	);

	QNetworkRequest request(url);
	m_WebCtrl.get(request);
}

void DownloadManager::fileDownloaded(QNetworkReply* pReply) {
	m_DownloadedData = pReply->readAll();
	//emit a signal
	pReply->deleteLater();
	emit downloaded();
}

QByteArray DownloadManager::downloadedData() const {
	return m_DownloadedData;
}