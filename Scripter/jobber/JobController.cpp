#include "JobController.h"
#include <jobber/JobDispatcher.h>

#include <log/Log.h>

using namespace Scripter::Communication;

JobController::JobController(const Pipe &readPipe, const Pipe &writePipe)
    : m_workLocal(true)
{
    JobDispatcher *job = new JobDispatcher(readPipe, writePipe);

    connect(this, SIGNAL(operate(std::string)), job, SLOT(doWork(std::string)));
    connect(this, SIGNAL(operate(uint, std::string)), job, SLOT(doWork(uint, std::string)));
    connect(this, SIGNAL(terminate()), job, SLOT(terminate()));
    connect(&m_listenerThread, SIGNAL(finished()), job, SLOT(deleteLater()));
    connect(job, SIGNAL(jobError(std::string)), this, SLOT(handleError(std::string)));
    connect(job, SIGNAL(jobError(uint, std::string)), this, SLOT(handleError(uint, std::string)));
    connect(job, SIGNAL(resultReady(std::string)), this, SLOT(handleResult(std::string)));
    connect(job, SIGNAL(resultReady(uint,std::string)), this, SLOT(handleResult(uint,std::string)));
    job->moveToThread(&m_listenerThread);
    m_listenerThread.start();

    m_nodder = new Nodder();
    connect(m_nodder, SIGNAL(changeWorkPlace(bool)), this, SLOT(jobRedirect(bool)));
    connect(m_nodder, SIGNAL(jobResult(std::string)), this, SLOT(handleResult(std::string)));
    connect(m_nodder, SIGNAL(jobRequest(uint,std::string)),
            this, SLOT(doJob(uint, std::string)));
    connect(this, SIGNAL(resultAcquired(uint,std::string)),
            m_nodder, SLOT(handleLocalResult(uint,std::string)));
    connect(this, SIGNAL(resultError(uint,std::string)),
            m_nodder, SLOT(handleLocalError(uint,std::string)));
    connect(this, SIGNAL(operateRemote(std::string)),
            m_nodder, SLOT(handleLocalRequest(std::string)));

}

JobController::~JobController() {
    emit terminate();
    m_listenerThread.exit();
    m_listenerThread.wait();

    delete m_nodder;
}

void JobController::start() {
    m_nodder->start();
}

void JobController::jobRedirect(bool local) {
    LOG("Got jobRedirect: " << (local ? "local":"remote"));
    m_workLocal = local;
}

void JobController::dispatchJob(const std::string &job) {
    LOG("dispatchJob: " << job);
    if (m_workLocal)
        emit operate(job);
    else
        emit operateRemote(job);
}

void JobController::handleResult(const std::string &result) {
    LOG("handleResult: " << result);
    emit resultAcquired(result);
}

void JobController::handleError(const std::string &error) {
    LOG("handleError");
    emit resultError(error);
}

void JobController::doJob(unsigned id, const std::string &job) {
    LOG("doJob");
    emit operate(id, job);
}

void JobController::handleResult(unsigned id, const std::string &result) {
    LOG("handleResult");
    emit resultAcquired(id, result);
}

void JobController::handleError(unsigned id, const std::string &error) {
    LOG("handleResult id=" << id);
    emit resultError(id, error);
}

