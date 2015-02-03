#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <string>

enum class ProtocolMessage : uint32_t {
    HelloClient = 0,
    HelloUI,
    Redirect,
    Id,
    Nodes,
    NewNode,
    JobNew,
    JobDone,
    JobRequest,
    JobResult,
    BeOverlord,
    NewOverlord,
    UnknownMessage
};

namespace ProtocolText {
extern const std::string helloClient;
extern const std::string helloUi;

extern const std::string redirect;
extern const std::string id;
extern const std::string nodes;
extern const std::string newNode;

extern const std::string jobRequest;
extern const std::string jobResult;
extern const std::string jobNew;
extern const std::string jobDone;
}

#endif // PROTOCOL_H
