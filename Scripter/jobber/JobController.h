#ifndef JOBBER_H
#define JOBBER_H

#include <QObject>
#include <QThread>

#include <comm/Pipe.h>

#include <string>

class JobController : public QObject
{
    Q_OBJECT
    QThread m_listenerThread;

public:
    JobController(const Scripter::Communication::Pipe &readPipe,
                  const Scripter::Communication::Pipe &writePipe);
    virtual ~JobController();

signals:
    void operate(const std::string &job);
    void terminate();
    void resultAcquired(const std::string &result);
    void resultError(const std::string &error);

public slots:
    void handleResult(const std::string &result);
    void handleError(const std::string &error);
    void dispatchJob(const std::string &job);

};

#endif // JOBBER_H
