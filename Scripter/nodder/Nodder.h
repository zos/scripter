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
    void jobResult(QString result);
    void jobRequest(unsigned id, const std::string &job);
    void noAvailableHosts();
    void workerAvailable();

public slots:
    void start();
    void onConnected();
    void onError(QAbstractSocket::SocketError);
    void extractMessageServer(QTcpSocket *socket);
    void extractMessageSocket();
    void onStatusChange(QAbstractSocket::SocketState state);
private:
    bool tryConnect();
    bool handleClientMessage();
    bool handleServerMessage(QTcpSocket *socket, const Message &m);
    bool handleMessageFromClient();
    bool handleMessageFromOverlord();

    bool sendMessage(QTcpSocket *socket, const Message &m);
    bool sendHelloClient(QTcpSocket *socket);
    bool extractMessage(QTcpSocket *socket, Message &m);

    void startOverlord();
    void startNode();

    Configuration m_conf;

    TcpServer *m_server;
    QTcpSocket *m_socket;

    Overlord *m_overlord;
    Configuration::HostInfo m_currentOverlord;
    bool m_connected;
    bool m_overlordFound;

    unsigned m_id;
    std::deque<Configuration::HostInfo> m_connectionTries;
    std::deque<Configuration::HostInfo> m_knownHosts;
    Message m_clientMessage;
    Message m_serverMessage;
};

#endif // NODDER_H
