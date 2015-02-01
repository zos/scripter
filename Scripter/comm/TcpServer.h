#ifndef TCPSERVER_H
#define TCPSERVER_H


#include <comm/TcpSocket.h>
#include <comm/Message.h>

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QString>

#include <cstddef>
#include <map>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(const QString &address, uint16_t port);

    void start();

    ~TcpServer();

signals:
    void newPeer(QTcpSocket *);
    void readyRead(QTcpSocket *);
    void serverError(const QString &error);
    void peerClosed(QTcpSocket *peer);

public slots:
    void newConnection();
    void onAcceptError(const QAbstractSocket::SocketError &error);
    void socketClosed(QTcpSocket* peer);
    void socketRead(QTcpSocket *socket);

private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_sockets;
    Message m_message;
    std::map<unsigned, TcpSocket*> m_peers;

    QString m_address;
    uint16_t m_port;

};

#endif // TCPSERVER_H
