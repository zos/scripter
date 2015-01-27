/*
 *      Author: Zofia Abramowska
 *		File: Job.h
 */

#pragma once

#include <stdint.h>
#include <vector>
#include <string>

namespace Scripter {

class Job {
public:
    Job(uint64_t jobId, std::vector<char> data, std::string path);
    std::string getFileName() {
        return m_file;
    }
    std::vector<char> getResult();
    virtual ~Job();
private:
    uint64_t m_jobId;
    std::vector<char> m_job;
    std::string m_file;
    std::string m_path;
};

} /* namespace Scripter */
