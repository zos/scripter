#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>

class TcpSocket : public QObject
{
    Q_OBJECT
public:
    explicit TcpSocket();
    explicit TcpSocket(QTcpSocket *socket);
    virtual ~TcpSocket();
signals:
    void readyRead(TcpSocket *tcp);
    void error(TcpSocket *socket, const QAbstractSocket::SocketError &error);
    void statusChanged(TcpSocket *socket, const QAbstractSocket::SocketState &state);
public slots:
    void readyRead();
    void onError(const QAbstractSocket::SocketError &error);
    void onStatusChange(const QAbstractSocket::SocketState &state);
private:
    QTcpSocket *m_socket;

};

#endif // TCPSOCKET_H
