#ifndef NODDER_H
#define NODDER_H

#include <QObject>
#include <QTcpSocket>

#include <comm/TcpServer.h>

class Nodder : public QObject
{
    Q_OBJECT
public:
    explicit Nodder(QObject *parent = 0);

signals:
    void jobResult(const QString &result);
    void noAvailableHosts();

public slots:
    void start(const QString &address, uint16_t port);
    void onConnected();
    void onError(QAbstractSocket::SocketError);
    void onReadyRead();
private:
    void tryConnect();
    void sendHelloClient();
    QList<QString> m_hosts;
    unsigned int m_hostPort;
    unsigned int m_nodePort;
    unsigned int m_overlordPort;
    TcpServer *m_server;
    QTcpSocket *m_socket;
    QString m_currentOverlord;
    bool m_connected;

};

#endif // NODDER_H
