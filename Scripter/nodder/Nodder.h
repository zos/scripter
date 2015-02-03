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
    void jobResult(std::string result);
    void jobRequest(unsigned id, const std::string &job);
    void changeWorkPlace(bool local);
    void workerAvailable();

public slots:
    void start();

    void updateNetwork();

    void onStatusChange(QAbstractSocket::SocketState state);
    void extractMessageServer(QTcpSocket *socket);
    void extractMessageSocket();
    void onError(QAbstractSocket::SocketError);

    void handleLocalRequest(const std::string &job);
    void handleLocalResult(unsigned id, const std::string &result);
    void handleLocalError(unsigned id, const std::string &error);
private:

    bool tryConnect();
    bool handleClientMessage();
    bool handleServerMessage(QTcpSocket *socket, const Message &m);
    bool handleMessageFromClient();
    bool handleMessageFromOverlord();

    void handleNewJob();
    void handleJobResult();

    bool sendMessage(QTcpSocket *socket, const Message &m);
    bool sendHelloClient(QTcpSocket *socket);
    bool sendJobRequest();
    bool sendJobDone(unsigned id, std::string result);
    bool extractMessage(QTcpSocket *socket, Message &m);

    void startOverlord();
    void stopOverlord();
    void startNode();

    unsigned findNewJobId();

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
    std::map<unsigned, std::string> m_assignedJobs;
    std::map<unsigned, std::string> m_sentJobs;
    Message m_clientMessage;
    Message m_serverMessage;
};

#endif // NODDER_H
