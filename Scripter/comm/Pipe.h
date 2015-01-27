/*
 *      Author: Zofia Abramowska
 *		File: Pipe.h
 */

#pragma once

#include <sys/types.h>

#include <vector>

namespace Scripter {
namespace Communication {

enum class PipeType {
    READ_PIPE,
    WRITE_PIPE
};

class Pipe {
public:
    Pipe(PipeType type);
    Pipe(const Pipe &other);

    Pipe& operator=(const Pipe &other);
    virtual ~Pipe();

    int getPipe();

    std::vector<char> read(std::size_t amount);
    std::size_t write(std::vector<char> data);
private:
    void setType();

    int m_fd[2];
    bool m_once;
    PipeType m_type;
    pid_t m_creatorPid;
};

} /* namespace Communication */
} /* namespace Scripter */


