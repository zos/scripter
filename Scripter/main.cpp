#include "mainwindow.h"
#include <QApplication>
#include <comm/Pipe.h>
#include <jobber/Job.h>
#include <log/Log.h>

#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <cstring>

using namespace Scripter;

int main(int argc, char *argv[])
{
    Communication::Pipe m_readPipe(Communication::PipeType::READ_PIPE);
    Communication::Pipe m_writePipe(Communication::PipeType::WRITE_PIPE);

    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    }
    if (pid) {
        QApplication a(argc, argv);

        MainWindow w(m_readPipe, m_writePipe);
        w.show();

        return a.exec();

    } else {
        //child
        //Child has different types for pipes
        uint64_t job_id = 1;
        while(true) {
            int read_fd = m_writePipe.getPipe();
            int write_fd = m_readPipe.getPipe();

            std::size_t jobSize = 0;
            LOG("Child: waiting for job");
            int ret = read(read_fd, &jobSize, sizeof(jobSize));
            if (ret == 0) {
                LOG("Parent disconnected");
                return 1;
            }
            if (ret == -1) {
                LOG("Error reading from pipe: " << strerror(errno));
                return 2;
            }

            std::vector<char> jobBuffer(jobSize, '\0');
            ret = read(read_fd, jobBuffer.data(), jobSize);
            if (ret == 0) {
                LOG("Parent disconnected");
                return 1;
            }
            if (ret == -1) {
                LOG("Error reading from pipe: " << strerror(errno));
                return 2;
            }

            LOG("Child: received job");
            Scripter::Job job(job_id, jobBuffer, "/tmp/");
            auto result = job.getResult();
            auto resultLen = result.size();
            ret = write(write_fd, &resultLen, sizeof(std::size_t));
            if (ret == -1) {
                LOG("Error writing to pipe: " << strerror(errno));
                return 2;
            }

            ret = write(write_fd, result.data(), result.size());
            if (ret == -1) {
                LOG("Error writing to pipe: " << strerror(errno));
                return 2;
            }
            LOG("Sent result");
            job_id++;
        }
        return 0;
    }
}
