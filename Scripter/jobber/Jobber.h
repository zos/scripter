/*
 *      Author: Zofia Abramowska
 *		File: Jobber.h
 */

#pragma once

namespace Scripter {

class Jobber {
public:
    Jobber();
    void waitForJob();
    void processJob();
    void sendJobResult();
    virtual ~Jobber();
};

} /* namespace Scripter */
