#ifndef MESSAGE_H
#define MESSAGE_H

#include <comm/Protocol.h>
#include <stddef.h>

#include <string>
#include <ostream>
#include <vector>

class Message;
std::ostream &operator<<(std::ostream &os, const Message &m);

class Message
{
public:
    typedef uint32_t TextSize;
    typedef uint32_t DataSize;
    Message();
    Message(ProtocolMessage type,
            const std::string &textData = std::string(),
            const std::vector<char> &binaryData = std::vector<char>());

    //Message(ProtocolMessage type, const std::string &textData);

    ProtocolMessage getType() const;

    void setTextSize(TextSize size);
    TextSize getTextSize() const {
        return m_textDataSize;
    }

    void setBinarySize(DataSize size);
    DataSize getBinarySize() const {
        return m_binaryDataSize;
    }

    void addData(const std::vector<char> &data);
    std::vector<char> getData() const {
        return m_data;
    }

    std::vector<char> getBinaryData() const {
        return std::vector<char>(m_data.begin() + m_textDataSize,
                                 m_data.begin() + m_textDataSize + m_binaryDataSize);
    }

    std::string getCommand() const;
    std::vector<std::string> getParams() const;

    bool isDone() const;
    bool isSet() const;
    void reset();


private:
    mutable ProtocolMessage m_type;
    DataSize m_textDataSize;
    TextSize m_binaryDataSize;
    std::vector<char> m_data;
};

#endif // MESSAGE_H
