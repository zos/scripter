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

    qintptr socketDescriptor() {
        return m_socket->socketDescriptor();
    }

    QTcpSocket *getSocket() {
        return m_socket;
    }

    void abort() {
        m_socket->abort();
    }

signals:
    void readyRead(TcpSocket *tcp);
    void errorSocket(QTcpSocket *socket, const QAbstractSocket::SocketError &error);
    void statusChanged(QTcpSocket *socket, const QAbstractSocket::SocketState &state);
public slots:
    void readyRead();
    void onError(const QAbstractSocket::SocketError &error);
    void onStatusChange(const QAbstractSocket::SocketState &state);
private:
    QTcpSocket *m_socket;

};

#endif // TCPSOCKET_H
