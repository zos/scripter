#include "Nodder.h"

#include <comm/Config.h>
#include <comm/Protocol.h>
#include <comm/Serialization.h>
#include <log/Log.h>

#include <QHostAddress>
#include <QSettings>

#include <sstream>


Nodder::Nodder(QObject *parent) :
    QObject(parent), m_server(NULL), m_connected(false)
{
    m_conf.loadFromFile(Config::configPath);
    m_socket = new QTcpSocket();
    for (auto &node : m_conf.getHosts()) {
        m_connectionTries.push_back(node);
    }
}

bool Nodder::tryConnect() {
    const int timeout = 2 * 1000;
    m_socket->abort();
    while(!m_connectionTries.empty()) {
        auto &node = m_connectionTries.front();
        m_socket->connectToHost(node.address, m_conf.getHostPort());
        LOG("Trying to connect to: " << node.name.toStdString());
        if (m_socket->waitForConnected(timeout)) {

            LOG("Connected to: " << node.name.toStdString());
            connect(m_socket, SIGNAL(readyRead()), this, SLOT(extractMessageSocket()));
            connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
            connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChange(QAbstractSocket::SocketState)));
            if (!sendHelloClient(m_socket)) {
                //Couldn't send hello
                m_socket->abort();
                m_connectionTries.pop_front();
                continue;
            }
            m_currentOverlord = node;
            return true;
        } else {
            LOG("Couldn't connect to: " << node.name.toStdString() << " : " << m_socket->errorString().toStdString());
        }
        m_connectionTries.pop_front();
    }
    return false;
}

bool Nodder::sendHelloClient(QTcpSocket *socket) {
    Message m;
    std::string textData = ProtocolText::helloClient;
    m.setBinarySize(0);
    m.setTextSize(textData.size());

    return sendMessage(socket, m);
}

void Nodder::start() {
    LOG("Starting node");
    if (tryConnect()) {
        LOG("Connection estabilished");
        m_connected = true;
    } else {
        LOG("No host available");
        emit noAvailableHosts();
        startOverlord();
    }
    startNode();
}

void Nodder::startNode() {
    m_server = new TcpServer(m_conf.getAddress(), m_conf.getNodePort());
    connect(m_server, SIGNAL(readyRead(QTcpSocket*)), this, SLOT(extractMessageServer(QTcpSocket*)));
    m_server->start();
}

void Nodder::startOverlord() {
    m_overlord = new Overlord(m_conf.getAddress(), m_conf.getOverlordPort());
    m_overlord->start();
}

void Nodder::onConnected() {

}

void Nodder::onError(QAbstractSocket::SocketError) {

}

void Nodder::extractMessageSocket() {
    extractMessage(m_socket);
    //possible job-new, redirect, id, nodes

}

void Nodder::extractMessageServer(QTcpSocket *socket) {
    //possible hello-client, redirect
    extractMessage(socket);
    if (m_message.isDone()) {
        (void)handleClientMessage(socket, m_message);
        m_message.reset();
    }
}

bool Nodder::extractMessage(QTcpSocket *socket) {
    if (!m_message.isSet()) {
        unsigned headerSize = sizeof(Message::TextSize) + sizeof(Message::DataSize);
        if (socket->bytesAvailable() < (headerSize))
            return true;
        std::vector<char> data(headerSize, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), headerSize);
        if (ret == -1) {
            //Qt docs says, that disconnection will be returned as -1
           return false;
        }
        Message::TextSize textSize;
        Message::DataSize binarySize;

        textSize = deserialize<Message::TextSize>(data);
        binarySize = deserialize<Message::DataSize>(data);

        m_message.setTextSize(textSize);
        m_message.setBinarySize(binarySize);
    }
    if (!m_message.isDone()) {
        qint64 dataSize = socket->bytesAvailable();
        std::vector<char> data(dataSize, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), dataSize);
        if (ret == -1) {
            return false;
        }
        m_message.addData(data);
    }
    return true;
}

bool Nodder::handleClientMessage(QTcpSocket *socket, const Message &m) {
    ProtocolMessage type = m.getType();
    switch(type) {
    case ProtocolMessage::HelloClient: {
        std::string textData = m_currentOverlord.address.toStdString()
                              + " " + std::to_string(m_conf.getOverlordPort());
        Message response(ProtocolMessage::Redirect, textData);
        return sendMessage(socket, response);
    }
    case ProtocolMessage::Redirect:
        throw std::runtime_error("Not yet implemented!");
    default:
        return false;
    }

    return true;
}

bool Nodder::sendMessage(QTcpSocket *socket, const Message &m) {
    if (socket->write(m.getData().data(), m.getData().size()) == -1) {
        socket->abort();
        return false;
    }
    return true;
}


