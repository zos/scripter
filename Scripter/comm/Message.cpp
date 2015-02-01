#include "Message.h"

#include <sstream>
#include <string>
#include <stdexcept>

std::string protocolTypeToMsg(ProtocolMessage type) {
    std::string command;
    switch(type) {
    case ProtocolMessage::HelloClient:
        command = ProtocolText::helloClient;
        break;

    case ProtocolMessage::Redirect:
        command = ProtocolText::redirect;
        break;
    default:
        throw std::runtime_error("Not yet implemented!");
    }

    return command;
}


Message::Message() : m_type(ProtocolMessage::UnknownMessage), m_textDataSize(0), m_binaryDataSize(0)
{
}

Message::Message(ProtocolMessage type,
                 const std::string &textData,
                 const std::vector<char> &binaryData)
{
    std::string command = protocolTypeToMsg(type);
    command += " ";

    m_type = type;
    m_textDataSize = textData.length();
    m_binaryDataSize = binaryData.size();

    m_data.reserve(command.length() + m_textDataSize + m_binaryDataSize);
    m_data.insert(m_data.end(), command.begin(), command.end());
    m_data.insert(m_data.end(), textData.begin(), textData.end());
    m_data.insert(m_data.end(), binaryData.begin(), binaryData.end());
}

/*Message::Message(ProtocolMessage type, const std::string &textData) {
    std::string command = protocolTypeToMsg(type);
    command += " ";

    m_type = type;
    m_textDataSize = textData.length();
    m_binaryDataSize = 0;

    m_data.reserve(command.length() + textData.length());
    m_data.insert(m_data.end(), command.begin(), command.end());
    m_data.insert(m_data.end(), textData.begin(), textData.end());

}*/

ProtocolMessage Message::getType() const {
    std::string command = getCommand();

    if (m_type == ProtocolMessage::UnknownMessage) {
        if (command == ProtocolText::helloClient) {
            m_type = ProtocolMessage::HelloClient;
        }
        if (command == ProtocolText::redirect) {
            m_type = ProtocolMessage::Redirect;
        }

        if (command == ProtocolText::jobRequest) {
            m_type = ProtocolMessage::JobRequest;
        }

        if (command == ProtocolText::jobNew) {
            m_type = ProtocolMessage::JobNew;
        }

        if (command == ProtocolText::jobDone) {
            m_type = ProtocolMessage::JobDone;
        }

        if (command == ProtocolText::jobResult) {
            m_type = ProtocolMessage::JobResult;
        }
    }

    return m_type;

}

void Message::setTextSize(uint32_t size) {
    m_textDataSize = size;
}

void Message::setBinarySize(uint32_t size) {
    m_binaryDataSize = size;
}

void Message::addData(const std::vector<char> &data) {
    m_data.reserve(m_data.size() + data.size());
    m_data.insert(m_data.end(), data.begin(), data.end());
}

bool Message::isSet() const {
    return m_textDataSize != 0;
}

bool Message::isDone() const {
    return isSet() && m_data.size() == (m_textDataSize + m_binaryDataSize);
}

std::string Message::getCommand() const {
    std::string allParams(m_data.begin(), m_data.begin() + m_textDataSize);
    std::stringstream cmdSS(allParams);

    std::string command;
    cmdSS >> command;

    return command;
}

std::vector<std::string> Message::getParams() const {
    std::vector<std::string> params;
    if (!isSet() || !isDone())
        return params;
    std::string param;
    std::string allParams(m_data.begin(), m_data.begin() + m_textDataSize);
    std::stringstream paramSS(allParams);
    while(paramSS >> param) {
        params.push_back(param);
    }
    return params;
}

void Message::reset() {
    m_textDataSize = 0;
    m_textDataSize = 0;
    m_type = ProtocolMessage::UnknownMessage;
    m_data.clear();
}
