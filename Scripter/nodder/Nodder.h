#ifndef NODDER_H
#define NODDER_H

#include <QObject>
#include <QTcpSocket>

#include <comm/Message.h>
#include <comm/TcpServer.h>
#include <comm/Config.h>

#include <nodder/Overlord.h>

#include <deque>

class Nodder : public QObject
{
    Q_OBJECT
public:
    explicit Nodder(QObject *parent = 0);

signals:
    void jobResult(const QString &result);
    void noAvailableHosts();
    void workerAvailable();

public slots:
    void start();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
    void extractMessageServer(QTcpSocket *socket);
    void extractMessageSocket();
private:
    bool tryConnect();
    bool handleClientMessage(QTcpSocket *socket, const Message &m);
    bool sendMessage(QTcpSocket *socket, const Message &m);
    bool sendHelloClient(QTcpSocket *socket);
    bool extractMessage(QTcpSocket *socket);

    void startOverlord();
    void startNode();

    Configuration m_conf;

    TcpServer *m_server;
    QTcpSocket *m_socket;

    Overlord *m_overlord;
    Configuration::HostInfo m_currentOverlord;
    bool m_connected;

    std::deque<Configuration::HostInfo> m_connectionTries;
    Message m_message;
};

#endif // NODDER_H
