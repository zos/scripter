#ifndef JOBBERLISTENER_H
#define JOBBERLISTENER_H

#include <QObject>
#include <string>

#include <comm/Pipe.h>

class JobDispatcher : public QObject
{
    Q_OBJECT
public:
    JobDispatcher(const Scripter::Communication::Pipe &readPipe,
                   const Scripter::Communication::Pipe &writePipe);

signals:
    void resultReady(const std::string &result);
    void resultReady(unsigned id, const std::string &result);
    void jobError(const std::string &error);
    void jobError(unsigned id, const std::string &error);
public slots:
    void doWork(const std::string &job);
    void doWork(unsigned id, const std::string &job);
    void terminate();

private:
    bool dispatchJob(const std::string &job, std::string &result);
    Scripter::Communication::Pipe m_read;
    Scripter::Communication::Pipe m_write;

};

#endif // JOBBERLISTENER_H
