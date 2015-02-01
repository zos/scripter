#include "TcpServer.h"

#include <comm/TcpSocket.h>

#include <QDataStream>
#include <QHostAddress>

#include <utility>

TcpServer::TcpServer(const QString &address, uint16_t port) : m_address(address), m_port(port) {
    m_server = new QTcpServer();
    connect(m_server, SIGNAL(newConnection()), this, SLOT(newConnection()));
    connect(m_server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(onAcceptError(QAbstractSocket::SocketError)));
}

TcpServer::~TcpServer() {
    for (auto &peer: m_peers) {
        peer.second->abort();
        delete peer.second;
    }
    m_peers.clear();
}
void TcpServer::start() {
    QHostAddress host(m_address);
    m_server->listen(host, m_port);
}

void TcpServer::newConnection() {
    TcpSocket *socket = new TcpSocket(m_server->nextPendingConnection());
    connect(socket, SIGNAL(readyRead(QTcpSocket*)), this, SLOT(socketRead(QTcpSocket*)));
    connect(socket, SIGNAL(socketClosed(QTcpSocket*)), this, SLOT(socketClosed(QTcpSocket*)));
    m_peers.insert(std::make_pair(socket->socketDescriptor(), socket));
    emit newPeer(socket->getSocket());
}

void TcpServer::onAcceptError(const QAbstractSocket::SocketError &error) {
    //dunno
    switch (error) {
    case QAbstractSocket::SocketError::AddressInUseError:
        emit serverError("Address already in use");
        break;
    default:
        emit serverError("Unknown error");
        break;
    }
}

void TcpServer::socketRead(QTcpSocket *socket) {
    emit readyRead(socket);
}

void TcpServer::socketClosed(QTcpSocket* peer) {
    emit peerClosed(peer);
}
