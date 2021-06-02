#include "HttpHead.h"


HttpHead::HttpHead(QUrl _url, int _time, QString _modifiedSince, QObject* parent) : QThread(parent)
{
	// save parameter to class
	this->time = _time;
	this->url = _url;
	if (!_modifiedSince.isEmpty()) {
		this->modifiedSince = _modifiedSince;
	}

}

void HttpHead::run() {
	// create in thread variables
	QNetworkRequest request;
	QNetworkAccessManager m_WebCtrl;
	QEventLoop loop;
	QTimer timeout;

	// forge request
	request.setUrl(this->url);
	request.setHeader(QNetworkRequest::IfModifiedSinceHeader, this->modifiedSince);

	// set timeout
	timeout.setSingleShot(true);

	// event for network timeout
	connect(&timeout, &QTimer::timeout, &loop, &QEventLoop::quit);

	// event for complited download
	connect(
		&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(fileDownloaded(QNetworkReply*))
	);

	// at completed download exit loop
	connect(this, SIGNAL(downloaded(QByteArray)), &loop, SLOT(quit()));
	
	// send network request
	m_WebCtrl.get(request);

	// start time for timeout request
	timeout.start(this->time);
	loop.exec(); // loop
	
	// at loop end or at loop exitted for download completed

	if (timeout.isActive()) { // at download completed exit loop
		timeout.stop();
	}
	else { // at loop end, Error timeout
		qDebug("Error downloading file (timeout)");
		m_WebCtrl.deleteLater();
	}
}

void HttpHead::fileDownloaded(QNetworkReply* pReply) {
	QVariant status = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	

	switch (status.toInt()) {
		case 200: // file modified, new content to download
			qDebug() << "HttpHead: File modified";
			this->m_DownloadedData = pReply->readAll();
			break;
		case 304:// file not modified, no content to download
			qDebug() << "HttpHead: File NOT modified";
			this->m_DownloadedData = QByteArray(1,'1');
			break;
		default:// other http error
			qDebug() << "HttpHead: HTTP code "<< status.toInt() <<": " << status.toString();
			break;
	}

	pReply->deleteLater();

	// data if downloaded | null otherwise
	emit downloaded(this->m_DownloadedData);
	
}
