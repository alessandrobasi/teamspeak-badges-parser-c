#include "HttpHead.h"


HttpHead::HttpHead(QUrl url, int _time, QString modifiedSince, QObject* parent) : QThread(parent)
{
	// save parameter to class
	this->time = _time;
	this->url = url;
	if (!modifiedSince.isEmpty()) {
		this->modifiedSince = modifiedSince;
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

	// complited download
	connect(
		&m_WebCtrl, SIGNAL(finished(QNetworkReply*)),
		this, SLOT(fileDownloaded(QNetworkReply*))
	);

	// at completed download exit loop
	connect(this, SIGNAL(downloaded(QByteArray)), &loop, SLOT(quit()));
	
	// send network request
	m_WebCtrl.get(request);

	// sleep
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

	//end
}

void HttpHead::fileDownloaded(QNetworkReply* pReply) {
	QVariant status = pReply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	qDebug() << "Download end!";

	switch (status.toInt()) {
		case 200:
			qDebug() << "File modified";
			this->m_DownloadedData = pReply->readAll();
			break;
		case 304:
			qDebug() << "File NOT modified";
			break;
		default:
			qDebug() << "HTTP code error: " << status.toString();
			break;
	}

	pReply->deleteLater();

	qDebug() << "inizio emit ";
	// data if downloaded | null otherwise
	emit downloaded(this->m_DownloadedData);
	qDebug() << "fine emit ";
	
}
