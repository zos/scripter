#include "Nodder.h"

#include <comm/Config.h>

#include <QHostAddress>
#include <QSettings>


Nodder::Nodder(QObject *parent) :
    QObject(parent), m_server(NULL), m_connected(false)
{
    QSettings settings(QString::fromStdString(Scripter::Config::configPath), QSettings::IniFormat);
    settings.beginGroup("HOSTS");
    const QList<QString> allNodes = settings.childKeys();
    for (const auto& node : allNodes) {
        m_hosts << settings.value(node).toString();
    }
    settings.endGroup();
    m_hostPort = settings.value("NODE/hostPort").toUInt();
    m_nodePort = settings.value("NODE/selfPort").toUInt();
    m_overlordPort = settings.value("NODE/controllerPort").toUInt();

    m_socket = new QTcpSocket();
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void Nodder::onReadyRead() {
    // if state is - OverlordNotConfirmed, assure that reponse is Redirect and set to OverlordConfirmed,
    //               other messages are dropped
}

bool Nodder::tryConnect() {
    const int timeout = 2 * 1000;
    m_socket->abort();
    for (auto &node : m_hosts) {
        m_socket->connectToHost(node, m_hostPort);
        if (m_socket->waitForConnected(timeout)) {
            connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
            connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChange(QAbstractSocket::SocketState)));
            sendHelloClient();
            m_currentOverlord = node;
            return true;
        }
    }
    return false;
}

void Nodder::start(const QString &address, uint16_t port) {
    if (tryConnect()) {
        m_connected = true;
    } else {
        emit noAvailableHosts();
    }
}
