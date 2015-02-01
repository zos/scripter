/*
 *      Author: Zofia Abramowska
 *		File: Job.cpp
 */

#include "Job.h"
#include <fstream>
#include <cstdlib>

#include <log/Log.h>

namespace Scripter {

const std::string outFileName = "out.txt";

Job::Job(uint64_t jobId, std::vector<char> data, std::string path) :m_jobId(jobId), m_job(data), m_path(path) {
    m_file = "job" + std::to_string(jobId);

    std::string filePath = m_path + m_file;
    std::ofstream file(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
    file.write(data.data(), data.size());

    if(!file)
        m_file = "";
}

std::vector<char> Job::getResult() {
    LOG("Get result from: " << m_path << m_file);
    std::string cmd = "/bin/sh -c \"perl " + m_path + m_file + " > " + m_path + outFileName + " 2>&1\"";
    LOG("Running command: " << cmd);
    int ret = system(cmd.c_str());
    std::ifstream fin(m_path + outFileName);
    std::vector<char> result;
    if (!fin || ret != 0) {
        LOG("Error reading result");
        return result;
    }

    fin.seekg (0, std::ios::end);
    std::ios::pos_type length = fin.tellg();
    fin.seekg (0, std::ios::beg);

    // allocate memory:
    result.resize(length, '0');

    // read data as a block:
    fin.read (result.data(), length);
    fin.close();
    return result;
}

Job::~Job() {
    // TODO Auto-generated destructor stub
}

} /* namespace Scripter */
