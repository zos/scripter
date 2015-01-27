/*
 *      Author: Zofia Abramowska
 *		File: Pipe.cpp
 */

#include "Pipe.h"

#include <unistd.h>
#include <sys/types.h>
#include <cstring>
#include <errno.h>
#include <stdexcept>

#include <log/Log.h>

namespace Scripter {
namespace Communication {

PipeType operator~(const PipeType &type) {
    switch(type) {
    case PipeType::READ_PIPE:
        return PipeType::WRITE_PIPE;
    case PipeType::WRITE_PIPE:
        return PipeType::READ_PIPE;
    }
}

Pipe::Pipe(PipeType type) : m_once(true), m_type(type), m_creatorPid(getpid()) {
    if (pipe(m_fd) == -1) {
        LOG("Error creating pipe: " << strerror(errno));
        throw std::runtime_error("Error creating pipe");
    }
}

Pipe::Pipe(const Pipe &other) :
    m_once(other.m_once),
    m_type(other.m_type),
    m_creatorPid(other.m_creatorPid)

{
    m_fd[0] = other.m_fd[0];
    m_fd[1] = other.m_fd[1];
}

Pipe& Pipe::operator=(const Pipe &other) {
    if (this == &other)
        return *this;

    m_once = other.m_once;
    m_type = other.m_type;
    m_creatorPid = other.m_creatorPid;
    m_fd[0] = other.m_fd[0];
    m_fd[1] = other.m_fd[1];

    return *this;
}

void Pipe::setType() {
    if (m_once && m_creatorPid != getpid()) {
        //The other endpoint gets other type than was created
        m_type = ~m_type;
        m_once = false;
    }
}

int Pipe::getPipe() {
    setType();
    switch(m_type) {
    case PipeType::READ_PIPE:
        close(m_fd[1]);
        return m_fd[0];
    case PipeType::WRITE_PIPE:
        close(m_fd[0]);
        return m_fd[1];
    default:
        close(m_fd[0]);
        close(m_fd[1]);
        return -1;
    }
}

std::vector<char> Pipe::read(std::size_t amount) {
    std::vector<char> buffer;
    setType();
    if (m_type == PipeType::WRITE_PIPE || amount == 0)
        return buffer;
    buffer.resize(amount, '\0');

    int ret = ::read(m_fd[0], buffer.data(), amount);
    if (ret  == -1) {
        LOG("Error reading from pipe: " << strerror(errno));
        throw std::runtime_error("Error reading from pipe: " + std::string(strerror(errno)));
    }
    if (ret < static_cast<long int>(amount)) {
        buffer.resize(amount);
    }
    return buffer;
}

std::size_t Pipe::write(std::vector<char> buffer) {
    setType();
    if (m_type == PipeType::READ_PIPE || buffer.size() == 0)
        return 0;
    int ret = ::write(m_fd[1], buffer.data(), buffer.size());
    if (ret == -1) {
        LOG("Error writing to pipe: " << strerror(errno));
        throw std::runtime_error("Error writing to pipe: " + std::string(strerror(errno)));
    }
    return ret;
}

Pipe::~Pipe() {

}

} /* namespace Communication */
} /* namespace Scripter */
