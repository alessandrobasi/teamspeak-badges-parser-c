#include <QObject>
#include <QThread>
#include <QNetworkReply>
#include <QNetworkReply>
#include <QEventloop>
#include <QTimer>
#include <QUrl>

class HttpHead : public QThread
{
	Q_OBJECT

public:
	HttpHead(QUrl url, int _time, QString modifiedSince, QObject* parent = NULL);
	void run();

signals:
	void downloaded(QByteArray result);

private slots:
	void fileDownloaded(QNetworkReply* pReply);
private:
	QString modifiedSince = "Mon, 01 Jan 1990 00:00:00 GMT";
	QUrl url;
	int time;
	QByteArray m_DownloadedData = NULL;
};
