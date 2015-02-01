#ifndef OVERLORD_H
#define OVERLORD_H

#include <QObject>
#include <QString>

#include <comm/Message.h>
#include <comm/TcpServer.h>


#include <cstdint>
#include <map>
#include <list>
#include <cstdint>

class Overlord : public QObject
{
    Q_OBJECT

    struct JobInfo {
        QTcpSocket *owner;
        QTcpSocket *dispatcher;
        unsigned id;
        unsigned user_id;
    };

    struct PeerInfo {
        QTcpSocket *socket;
        unsigned jobsRequested;
        unsigned jobsDispatched;
    };

public:
    explicit Overlord(const QString &address, uint16_t port);
    void start();

signals:
    void jobRequest(unsigned, std::string);

public slots:
    void extractMessage(QTcpSocket *socket);
    void newPeer(QTcpSocket *socket);
    void peerClosed(QTcpSocket *socket);
private:

    void handleMessage(QTcpSocket *socket, const Message &m);
    bool sendMessage(QTcpSocket *socket, const Message &m);
    unsigned newJobId();
    JobInfo findJob(unsigned id, bool erase);
    QTcpSocket *findLowestLoad();
    QTcpSocket *findOwner();

    QString m_address;
    uint16_t m_port;
    TcpServer *m_server;

    unsigned m_selfLoad;
    unsigned m_jobId;

    std::map<qintptr, Message> m_clientMessage;
    std::vector<JobInfo> m_selfJobs;
    std::map<qintptr, std::list<JobInfo>> m_jobClientMapping;
    std::map<qintptr, std::list<JobInfo>> m_jobDispatchMapping;
    std::vector<PeerInfo> m_connectedPeers;
};

#endif // OVERLORD_H
