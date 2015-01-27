#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QList>
#include <QString>

#include <cstddef>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(const QString &address, uint16_t port);

signals:
    void newPeer(QTcpSocket *);

public slots:
    void newConnection();
    void onAcceptError(const QAbstractSocket::SocketError &error);
    void extractMessage(QTcpSocket *socket);


private:
    QTcpServer *m_server;
    QList<QTcpSocket*> m_sockets;

};

#endif // TCPSERVER_H
