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
    void jobError(const std::string &error);
public slots:
    void doWork(const std::string &job);
    void terminate();

private:
    Scripter::Communication::Pipe m_read;
    Scripter::Communication::Pipe m_write;

};

#endif // JOBBERLISTENER_H
