#include "Nodder.h"

#include <comm/Config.h>
#include <comm/Protocol.h>
#include <comm/Serialization.h>
#include <log/Log.h>
#include <utils.h>

#include <QHostAddress>
#include <QSettings>

#include <algorithm>
#include <sstream>

const QString overlord = "overlord";

Nodder::Nodder(QObject *parent) :
    QObject(parent), m_server(NULL), m_connected(false), m_overlordFound(false)
{
    m_conf.loadFromFile(Config::configPath);
    m_socket = new QTcpSocket();
    for (auto &node : m_conf.getHosts()) {
        m_connectionTries.push_back(node);
    }
}

bool Nodder::tryConnect() {
    LOG("tryConnect()");
    const int timeout = 2 * 1000;
    m_socket->abort();
    while(!m_connectionTries.empty()) {
        auto &node = m_connectionTries.front();
        m_socket->connectToHost(node.address, node.port);
        LOG("Trying to connect to: " << node.name.toStdString()
            << " address=<" << node.address.toStdString() << ">"
            << " port=<" << node.port << ">");
        if (m_socket->waitForConnected(timeout)) {

            LOG("Connected");
            if (node.name == overlord) {
                LOG("Connected to overlord!");
                m_overlordFound = true;
            } else {
                LOG(node.name.toStdString() << " " << overlord.toStdString());
            }
            connect(m_socket, SIGNAL(readyRead()), this, SLOT(extractMessageSocket()));
            connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
            connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChange(QAbstractSocket::SocketState)));
            if (!sendHelloClient(m_socket)) {
                //Couldn't send hello
                LOG("Failed to send hello");
                //m_socket->abort();
                m_connectionTries.pop_front();
                continue;
            }
            m_connectionTries.pop_front();
            return true;
        } else {
            LOG("Couldn't connect");
        }
        m_connectionTries.pop_front();
    }
    emit noAvailableHosts();
    return false;
}

void Nodder::onStatusChange(QAbstractSocket::SocketState state) {
    switch(state) {
    case QAbstractSocket::SocketState::ConnectedState:
        LOG("Socket connected!");
        break;
    case QAbstractSocket::SocketState::UnconnectedState:
        if (m_overlordFound)
            m_overlordFound = false;
        LOG("Socket unnconected!");
        break;
    case QAbstractSocket::SocketState::HostLookupState:
        LOG("Socket hostLookupState");
        break;
    case QAbstractSocket::SocketState::ConnectingState:
        LOG("Socket Connecting!");
        break;
    case QAbstractSocket::SocketState::BoundState:
        LOG("Socket Connecting!");
        break;
    case QAbstractSocket::SocketState::ListeningState:
        LOG("Socket Listening!");
        break;
    case QAbstractSocket::SocketState::ClosingState:
        LOG("Socket Closing!");
        break;
    }
}

bool Nodder::sendHelloClient(QTcpSocket *socket) {
    LOG("Sending hello-client");
    Message m(ProtocolMessage::HelloClient);

    return sendMessage(socket, m);
}

void Nodder::start() {
    if (m_connected)
        return;
    LOG("Starting node");
    if (tryConnect()) {
        LOG("Connection estabilished");
        m_connected = true;
    } else {
        LOG("No host available");

        startOverlord();
    }
    startNode();
}

void Nodder::startNode() {
    LOG("Starting Node");
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

void Nodder::onError(QAbstractSocket::SocketError error) {
    LOG("onError: " << m_socket->errorString().toStdString());

}

void Nodder::extractMessageSocket() {
    LOG("Ready to read from socket");
    extractMessage(m_socket, m_clientMessage);
    if (!m_clientMessage.isDone())
        return;
    //possible redirect - when connected to normal host
    //possible job-new, id, nodes - when connected to overlord
    if (!handleClientMessage()) {
        m_socket->close();
        disconnect(m_socket, SIGNAL(readyRead()), this, SLOT(extractMessageSocket()));
        disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
        disconnect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStatusChange(QAbstractSocket::SocketState)));
        m_connected = false;
        if(!tryConnect()) {
            LOG("No hosts available");
        } else {
            LOG("Connection estabilished");
        }
    }
    m_clientMessage.reset();
}

void Nodder::extractMessageServer(QTcpSocket *socket) {
    LOG("Some event on server");
    //possible hello-client, redirect
    extractMessage(socket, m_serverMessage);
    if (m_serverMessage.isDone()) {
        (void)handleServerMessage(socket, m_serverMessage);
        m_serverMessage.reset();
    }
}

bool Nodder::extractMessage(QTcpSocket *socket, Message &message) {
    if (!message.isSet()) {
        LOG("Reading header");
        unsigned headerSize = sizeof(Message::TextSize) + sizeof(Message::DataSize);
        if (socket->bytesAvailable() < (headerSize))
            return true;
        std::vector<char> data(headerSize, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), headerSize);
        if (ret == -1) {
            LOG("Error reading from socket");
            //Qt docs says, that disconnection will be returned as -1
            return false;
        }
        std::string textSize(data.begin(), data.begin() + sizeof(Message::TextSize));
        std::string binarySize(data.begin() + sizeof(Message::TextSize),
                               data.begin() + sizeof(Message::TextSize) + sizeof(Message::DataSize));

        LOG("Got textSize=" << textSize << " binarySize=" << binarySize);
        //textSize = deserialize<Message::TextSize>(data);
        //binarySize = deserialize<Message::DataSize>(data);

        message.setTextSize(strToUnsigned(textSize));
        message.setBinarySize(strToUnsigned(binarySize));
    }
    if (message.isSet() && !message.isDone()) {
        LOG("Reading data");
        qint64 dataSize = socket->bytesAvailable();
        unsigned dataMissing = message.getTextSize() + message.getBinarySize()
                - message.getData().size();
        qint64 dataToRead = std::min(static_cast<unsigned>(dataSize), dataMissing);
        std::vector<char> data(dataToRead, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), dataToRead);
        if (ret == -1) {
            LOG("Error reading from socket");
            return false;
        }
        message.addData(data);
    }
    return true;
}

bool Nodder::handleServerMessage(QTcpSocket *socket, const Message &m) {
    LOG("handleServerMessage()");
    ProtocolMessage type = m.getType();
    switch(type) {
    case ProtocolMessage::HelloClient: {
        LOG("Server got hello-client");
        std::string textData = m_currentOverlord.address.toStdString()
                              + " " + std::to_string(m_conf.getOverlordPort());
        Message response(ProtocolMessage::Redirect, textData);
        return sendMessage(socket, response);
    }
    case ProtocolMessage::Redirect:
        LOG("Server got redirect");
        throw std::runtime_error("Not yet implemented!");
    default:
        return false;
    }

    return true;
}

bool Nodder::handleMessageFromClient() {
    LOG("handleMessageFromClient()");
    ProtocolMessage type = m_clientMessage.getType();
    switch(type) {
    case ProtocolMessage::Redirect: {
        LOG("Client got redirect");
        std::vector<std::string> params = m_clientMessage.getParams();
        if (params.size() != 2) {
            LOG("Wrong amount of params for redirect message");
            return false;
        }

        QString overlordAddress = QString::fromStdString(params[0]);
        unsigned port = strToUnsigned(params[1]);
        if (port != m_conf.getOverlordPort()) {
            LOG("Redirect passed different overlord port than configuration");
        }
        LOG("Redirected to : <" << overlordAddress.toStdString() << "> port: <" << port << ">");
        m_connectionTries.push_front(Configuration::HostInfo(overlord, overlordAddress, port));
        return false;
    }
    default:
        return false;
    }

    return true;
}

bool Nodder::handleMessageFromOverlord() {
    LOG("handleMessageFromOverlord(): " << m_clientMessage);
    ProtocolMessage type = m_clientMessage.getType();
    switch(type) {
    case ProtocolMessage::Id: {
        LOG("Got id message from overlord");
        if (m_clientMessage.getParams().size() != 1) {
            LOG("Wrong amount of params for Id message");
            break;
        }
        unsigned id = strToUnsigned(m_clientMessage.getParams().at(0));
        m_id = id;
        break;
    }
    case ProtocolMessage::JobNew: {
        LOG("Got job-request from overlod");
        if (m_clientMessage.getParams().size() != 1) {
            LOG("Wrong amount of params for job-new message");
            return false;
        }
        if (m_clientMessage.getBinaryData().size() == 0) {
            LOG("Empty job received!");
            return false;
        }
        break;
    }
    case ProtocolMessage::JobResult: {
        LOG("Got job-result from overlod");
        if (m_clientMessage.getParams().size() != 1) {
            LOG("Wrong amount of params for job-result message");
            return false;
        }
        std::string result(m_clientMessage.getBinaryData().data(), m_clientMessage.getBinarySize());
        emit jobResult(QString::fromStdString(result));
        break;
    }
    default:
        return false;
    }

    return true;
}

bool Nodder::handleClientMessage() {
    LOG("handleClientMessage(): " << m_clientMessage);
    if (m_overlordFound) {
        return handleMessageFromOverlord();
    } else {
        return handleMessageFromClient();
    }
    m_clientMessage.reset();
}

std::string fixedUintToStr(unsigned uvalue, int digits = 4) {
    std::string result;
    while (digits-- > 0) {
        result += ('0' + uvalue % 10);
        uvalue /= 10;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

bool Nodder::sendMessage(QTcpSocket *socket, const Message &m) {
    LOG("Send message: " << m);
    std::string textLength = fixedUintToStr(m.getTextSize());
    std::string binaryLength = fixedUintToStr(m.getBinarySize());
    if (socket->write(textLength.c_str(), textLength.length()) == -1) {
        LOG("Failed to send text length");
        //socket->abort();
        return false;
    }

    if (socket->write(binaryLength.c_str(), binaryLength.length()) == -1) {
        LOG("Failed to send binary length");
        //socket->abort();
        return false;
    }

    if (socket->write(m.getData().data(), m.getData().size()) == -1) {
        LOG("Failed to send data");
        //socket->abort();
        return false;
    }
    return true;
}


