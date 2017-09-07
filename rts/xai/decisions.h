/*
 * Decisions.h
 *
 *  Created on: Aug 17, 2017
 *      Author: irvineje
 */

#ifndef RTS_XAI_DECISIONS_H_
#define RTS_XAI_DECISIONS_H_
#include <string>
#include <vector>

#include "decision.h"
namespace xai {

class Decisions {
    std::vector<Decision> decisions;
public:
    Decisions();
    Decisions(const std::string& decisionDirPath);
    virtual ~Decisions();
    std::vector<Decision>& get_decisions();
};

} /* namespace xai */

#endif /* RTS_XAI_DECISIONS_H_ */
