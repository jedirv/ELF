/*
 * Decision.h
 *
 *  Created on: Aug 17, 2017
 *      Author: irvineje
 */

#ifndef RTS_XAI_DECISION_H_
#define RTS_XAI_DECISION_H_
#include <string>
namespace xai {

class Decision {
    std::string action;
    int frame;
public:
    Decision();
    virtual ~Decision();
    Decision(const std::string& a, const int f);
    Decision(const std::string& path);
    std::string getAction(){
        return action;
    }
    int getFrame(){
        return frame;
    }
};

} /* namespace xai */

#endif /* RTS_XAI_DECISION_H_ */
