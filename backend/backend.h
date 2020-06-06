#pragma once

#include <memory>
#include <deque>

#include <QObject>
#include <QWebSocketServer>

class QWebSocketServer;
class QWebSocket;

class Backend : public QObject {
	Q_OBJECT

public:
	Backend(qint32 port, QObject* parent = nullptr);
	~Backend();

public slots:
	void onNewConnection() noexcept;

	void processTextMessage(const QString& message) noexcept;
	void processBinaryMessage(const QByteArray& message) noexcept;
	void disconnected() noexcept;

private:
	std::unique_ptr<QWebSocketServer> websocketServer;

	std::deque<QWebSocket*> connections;

};