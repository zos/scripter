#include <jobber/JobDispatcher.h>
#include <log/Log.h>

#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <vector>

const std::size_t BUFF_SIZE = 1024;

JobDispatcher::JobDispatcher(const Scripter::Communication::Pipe &readPipe,
                             const Scripter::Communication::Pipe &writePipe) :
    m_read(readPipe),
    m_write(writePipe)
{
}

void JobDispatcher::doWork(const std::string &job) {
    LOG("doWork");
    std::string result;
    if (dispatchJob(job, result))
        emit resultReady(result);
    else
        emit jobError("Couldn't dispatch job");
}

void JobDispatcher::doWork(unsigned id, const std::string &job) {
    LOG("doWork id=" << id);
    std::string result;
    if (dispatchJob(job, result))
        emit resultReady(id, result);
    else
        emit jobError(id, "Couldn't dispatch job");
}

bool JobDispatcher::dispatchJob(const std::string &job, std::string &result) {
    LOG("dispatchJob: " << job);
    int write_fd = m_write.getPipe();

    std::size_t to_write = job.length();
    int ret = write(write_fd, &to_write, sizeof(std::size_t));
    if (ret == -1) {
        LOG("Error writing to pipe: " << strerror(errno));
        return false;
    }

    ret = write(write_fd, job.c_str(), to_write);
    if (ret == -1) {
        LOG("Error writing to pipe: " << strerror(errno));
        return false;
    }

    std::vector<char> resultBuff;
    std::size_t buffSize = 0;

    int read_fd = m_read.getPipe();
    ret = read(read_fd, &buffSize, sizeof(buffSize));

    if (ret == -1) {
        LOG("Error reading from pipe: " << strerror(errno));
        return false;
    }

    resultBuff.resize(buffSize, '\0');
    ret = read(read_fd, resultBuff.data(), buffSize);

    if (ret == -1) {
        LOG("Error reading from pipe: " << strerror(errno));
        return false;
    }
    LOG("Got result: " << result);
    result = std::string(resultBuff.data(), buffSize);
    return true;
}



void JobDispatcher::terminate() {

}
