#ifndef JOBBER_H
#define JOBBER_H

#include <QObject>
#include <QThread>

#include <comm/Pipe.h>

#include <nodder/Nodder.h>

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
    void operate(unsigned id, const std::string &job);
    void operateRemote(const std::string &job);
    void resultAcquired(const std::string &result);
    void resultAcquired(unsigned id, const std::string &result);
    void resultError(const std::string &error);
    void resultError(unsigned id, const std::string &error);
    void terminate();

public slots:
    void handleResult(const std::string &result);
    void handleResult(unsigned id, const std::string &result);
    void handleError(const std::string &error);
    void handleError(unsigned id, const std::string &error);
    void dispatchJob(const std::string &job);
    void jobRedirect(bool local);
    void start();

    void doJob(unsigned id, const std::string &job);

private:
    Nodder *m_nodder;
    bool m_workLocal;

};

#endif // JOBBER_H
