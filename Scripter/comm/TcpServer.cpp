#include "TcpServer.h"
#include <comm/TcpSocket.h>

#include <QHostAddress>

TcpServer::TcpServer(const QString &address, uint16_t port) {
    m_server = new QTcpServer();
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onAcceptError(QAbstractSocket::SocketError)));

    QHostAddress host(address);
    m_server->listen(host, port);
}

void TcpServer::newConnection() {
    TcpSocket *socket = new TcpSocket(m_server->nextPendingConnection());
    connect(socket, SIGNAL(readyRead(TcpSocket*)), this, slot(extractMessage(TcpSocket*)));


}

void TcpServer::onAcceptError(const QAbstractSocket::SocketError &error) {
    //dunno
    switch (error) {
    case QAbstractSocket::SocketError::AddressInUseError:
        break;
    case QAbstractSocket::SocketError::
    }
}

void TcpServer::newConnection() {
    //We accept only hello message and if we're not an overlord, then we send redirect
}
