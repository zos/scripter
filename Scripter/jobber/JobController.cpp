#include "JobController.h"
#include <JobDispatcher.h>

#include <log/Log.h>

using namespace Scripter::Communication;

JobController::JobController(const Pipe &readPipe, const Pipe &writePipe)
{
    JobDispatcher *job = new JobDispatcher(readPipe, writePipe);

    connect(this, SIGNAL(operate(std::string)), job, SLOT(doWork(std::string)));
    connect(this, SIGNAL(terminate()), job, SLOT(terminate()));
    connect(&m_listenerThread, SIGNAL(finished()), job, SLOT(deleteLater()));
    connect(job, SIGNAL(jobError(std::string)), this, SLOT(handleError(std::string)));
    connect(job, SIGNAL(resultReady(std::string)), this, SLOT(handleResult(std::string)));
    job->moveToThread(&m_listenerThread);
    m_listenerThread.start();
}

JobController::~JobController() {
    emit terminate();
    m_listenerThread.exit();
    m_listenerThread.wait();
}

void JobController::dispatchJob(const std::string &job) {
    LOG("dispatchJob: " << job);
    emit operate(job);
}

void JobController::handleResult(const std::string &result) {
    LOG("handleResult: " << result);
    emit resultAcquired(result);
}

void JobController::handleError(const std::string &error) {
    emit resultError(error);
}
