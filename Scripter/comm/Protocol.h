#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstddef>

enum class ProtocolMessage : uint32_t {
    HelloClient = 0,
    Redirect,
    Nodes,
    NewNode,
    JobNew,
    JobDone,
    JobRequest,
    JobResult,
    BeOverlord,
    NewOverlord
};

#endif // PROTOCOL_H
