/*
 * Decision.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: irvineje
 */

#include "decision.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
namespace xai {
using namespace std;
Decision::Decision(const std::string& a, const int f){
    action = a;
    frame = f;
}
Decision::Decision() {
    // TODO Auto-generated constructor stub
    action = "unknown";
    frame = -1;
}
Decision::Decision(const std::string& path){
    std::ifstream in(path.c_str());
    std::string line;
    while (getline(in, line)) {
        std::string delimiter = "=";
        std::string key = line.substr(0, line.find(delimiter));
        std::string value = line.substr(line.find(delimiter)+1, line.length());
        if (key == "action"){
            action = value;
        }
        else if (key == "tick"){
            frame = atoi(value.c_str());
        }
    }
    if (action == "unknown"  || frame == -1){
        throw domain_error("problem loading decision file " + path);
    }
}
Decision::~Decision() {
    // TODO Auto-generated destructor stub
}

} /* namespace xai */
