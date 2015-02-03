#include "Overlord.h"

#include <comm/Serialization.h>
#include <utils.h>

#include <list>
#include <log/Log.h>

Overlord::Overlord(const QString &address, uint16_t port) :
    //TODO Configuration needed, so proper id's can be assigned
    m_address(address), m_port(port), m_server(NULL), m_selfLoad(0), m_jobId(0)
{
    LOG("Overlord created with params address: " << address.toStdString() << " port: " << port);
}

void Overlord::start() {
    LOG("Starting overlord");
    m_server = new TcpServer(m_address, m_port);
    connect(m_server, SIGNAL(readyRead(QTcpSocket*)), this, SLOT(extractMessage(QTcpSocket*)));
    connect(m_server, SIGNAL(newPeer(QTcpSocket*)), this, SLOT(newPeer(QTcpSocket*)));
    connect(m_server, SIGNAL(peerClosed(QTcpSocket*)), this, SLOT(peerClosed(QTcpSocket*)));
    m_server->start();
}

void Overlord::newPeer(QTcpSocket *socket) {
    //TODO put this after hello-client hello-ui, peers have two types
    LOG("New peer acuired, fd: " << socket->socketDescriptor());
}

void Overlord::peerClosed(QTcpSocket *socket) {
    //TODO map for peers needed
    LOG("Peer closed, fd: " << socket->socketDescriptor());
    qintptr peerDescr = socket->socketDescriptor();
    m_jobClientMapping.erase(peerDescr);
    m_jobDispatchMapping.erase(peerDescr);
    //TODO redispatch!
}

void Overlord::extractMessage(QTcpSocket *socket) {
    LOG("New message from fd: " << socket->socketDescriptor());
    Message &message = m_clientMessage[socket->socketDescriptor()];
    if (!message.isSet()) {
        unsigned headerSize = sizeof(Message::TextSize) + sizeof(Message::DataSize);
        if (socket->bytesAvailable() < (headerSize))
            return;
        std::vector<char> data(headerSize, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), headerSize);
        if (ret == -1 || ret == 0) {
            message.reset();
            //Qt docs says, that disconnection will be returned as -1
           return;
        }
        Message::TextSize textSize;
        Message::DataSize binarySize;

        textSize = deserialize<Message::TextSize>(data);
        binarySize = deserialize<Message::DataSize>(data);

        message.setTextSize(textSize);
        message.setBinarySize(binarySize);
    }
    if (!message.isDone()) {
        qint64 dataSize = socket->bytesAvailable();
        std::vector<char> data(dataSize, '\0');
        QDataStream in(socket);
        auto ret = in.readRawData(data.data(), dataSize);
        if (ret == -1) {
            return;
        }
        message.addData(data);
    }
    if (message.isDone()) {
        handleMessage(socket, message);
        message.reset();
    }
    return;
}

unsigned Overlord::newJobId() {
    if (m_jobId == std::numeric_limits<unsigned>::max())
        throw std::runtime_error("Not yet implemented");
    m_jobId++;
    return m_jobId;
}

QTcpSocket* Overlord::findLowestLoad() {
    unsigned lowestLoad = std::numeric_limits<unsigned>::max();
    QTcpSocket *chosen = NULL;
    for (auto &peerInfo: m_connectedPeers) {
        if (peerInfo.second.jobsDispatched < lowestLoad) {
            chosen = peerInfo.second.socket;
            lowestLoad = peerInfo.second.jobsDispatched;
        }
    }
    if (lowestLoad > m_selfLoad)
        return NULL;
    return chosen;
}

Overlord::JobInfo Overlord::findJob(unsigned id, bool erase) {
    auto it = m_jobDispatchMapping.find(id);
    if (it == m_jobDispatchMapping.end()) {
        JobInfo emptyJob;
        emptyJob.owner = NULL;
        return emptyJob;
    }
    JobInfo jobFound;
    std::list<JobInfo> &jobs = it->second;
    auto jobIt = jobs.begin();
    for (; jobIt != jobs.end(); ++jobIt) {
        if (jobIt->id == id) {
            jobFound = *jobIt;
            break;
        }
    }

    if (jobIt == jobs.end()) {
        JobInfo emptyJob;
        emptyJob.owner = NULL;
        return emptyJob;
    }

    if (erase) {
        jobs.erase(jobIt);
    }

    return jobFound;
}

void Overlord::handleJobRequest(QTcpSocket *socket, const Message &m) {
    JobInfo newJob;
    newJob.id = newJobId();
    newJob.owner = socket;
    auto dispatcher = findLowestLoad();
    m_connectedPeers[dispatcher->socketDescriptor()].jobsDispatched++;

    auto params = m.getParams();
    newJob.user_id = strToUnsigned(params[0]);
    newJob.dispatcher = dispatcher;
    if (dispatcher == NULL) {
        //no peers connected or we have the lowest load
        m_selfJobs.push_back(newJob);
        std::string job(m.getBinaryData().data());
        emit jobRequest(newJob.id, job);
    }
    auto ownerDescr = socket->socketDescriptor();
    auto dispDescr = dispatcher->socketDescriptor();

    m_jobClientMapping[ownerDescr].push_back(newJob);
    m_jobDispatchMapping[dispDescr].push_back(newJob);

    Message jobNew(ProtocolMessage::JobNew, std::to_string(newJob.id), m.getBinaryData());
    sendMessage(dispatcher, jobNew);
}

void Overlord::handleJobDone(QTcpSocket *socket, const Message &m) {
    auto params = m.getParams();
    unsigned jobId = strToUnsigned(params[0]);

    qintptr dispDescr = socket->socketDescriptor();
    auto it = m_jobDispatchMapping.find(dispDescr);
    if (it == m_jobDispatchMapping.end()) {
        //job not dispatched, ignoring
        //TODO : should be saved
        return;
    }
    JobInfo jobInfo = findJob(jobId, true);
    if (jobInfo.owner == NULL) {
        //no job owner (probably closed), ignoring
        //TODO : should be saved
        return;
    }
    QTcpSocket *owner = jobInfo.owner;

    Message jobResult(ProtocolMessage::JobResult, std::to_string(jobInfo.user_id),
                      m.getBinaryData());
    sendMessage(owner, jobResult);
}

void Overlord::handleMessage(QTcpSocket *socket, const Message &m) {

    ProtocolMessage type = m.getType();
    LOG("Handling message from, fd: " << socket->socketDescriptor()
        << " of type: " << m.getCommand());
    switch(type) {
    case ProtocolMessage::HelloClient: {
        PeerInfo newPeer;
        newPeer.socket = socket;
        newPeer.jobsDispatched = 0;
        newPeer.jobsRequested = 0;
        m_connectedPeers[socket->socketDescriptor()] = newPeer;
        //sendID();
        //sendNodes();
        break;
    }
    case ProtocolMessage::HelloUI: {
        break;
    }
    case ProtocolMessage::JobRequest: {
        handleJobRequest(socket, m);
        break;
    }
    case ProtocolMessage::JobDone: {
        handleJobDone(socket, m);
        break;
    }
    default:
        //ignore
        break;
    }

}

bool Overlord::sendMessage(QTcpSocket *socket, const Message &m) {
    LOG("Sending response to fd: " << socket->socketDescriptor()
        << " cmd: " << m.getCommand());
    if (socket->write(m.getData().data(), m.getData().size()) == -1) {
        LOG("Error on socket fd: " << socket->socketDescriptor());
        // TODO: should we?
        socket->abort();
        return false;
    }
    return true;
}
