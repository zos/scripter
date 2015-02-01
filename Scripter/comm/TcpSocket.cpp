#include "TcpSocket.h"

TcpSocket::TcpSocket(QTcpSocket *socket) :
    m_socket(socket)
{
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChange(QAbstractSocket::SocketState)));
}

TcpSocket::~TcpSocket() {
    delete m_socket;
}

void TcpSocket::onError(const QAbstractSocket::SocketError &error) {
    emit errorSocket(m_socket, error);
}

void TcpSocket::onStatusChange(const QAbstractSocket::SocketState &state) {
    emit statusChanged(m_socket, state);
}

void TcpSocket::readyRead() {

}
