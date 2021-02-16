#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class DownloadManager : public QObject {
	Q_OBJECT
public:
	DownloadManager(QUrl url, QObject* parent = 0);
	QByteArray downloadedData() const;

signals:
	void downloaded();

private slots:
	void fileDownloaded(QNetworkReply* pReply);
private:
	QNetworkAccessManager m_WebCtrl;
	QByteArray m_DownloadedData;
};