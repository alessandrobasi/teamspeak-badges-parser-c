#include <QThread>
#include <QNetworkReply>
#include <QEventloop>
#include <QTimer>
#include <QUrl>
/*
This class download file from url 
if it's time (HTTP If-Modified-Since) is newer than 
local file modified date
*/
class HttpHead : public QThread
{
	Q_OBJECT

public:
	HttpHead(QUrl _url, int _time, QString _modifiedSince = QString(), QObject* parent = NULL);
	void run();

signals:
	void downloaded(QByteArray result);

private slots:
	void fileDownloaded(QNetworkReply* pReply);

private:
	QString modifiedSince = "Mon, 01 Jan 1990 00:00:00 GMT";
	QUrl url;
	int time;
	QByteArray m_DownloadedData = QByteArray();
};
