/*
 * Decisions.cpp
 *
 *  Created on: Aug 17, 2017
 *      Author: irvineje
 */

#include "decisions.h"
#include "../engine/file_util.h"
#include <stdexcept>
#include <fstream>
#include <vector>
#include <iostream>
namespace xai {
using namespace std;
Decisions::Decisions() {
    // TODO Auto-generated constructor stub

}
Decisions::Decisions(const std::string& decisionDirPath){
    if (!FileUtils::is_valid_dir(decisionDirPath)){
        throw domain_error(decisionDirPath + " is not valid directory");
    }
    vector<std::string> filenames = FileUtils::list_files(decisionDirPath);
    vector<std::string>::const_iterator iter = filenames.begin();
    std::string aid_file_extension(".aid");
    while(iter != filenames.end()){
        if (FileUtils::has_filetype(*iter, aid_file_extension)){
            cout << "filename " << *iter << endl;
            //std::string root = FileUtils::get_file_root(*iter);
            std::string path(decisionDirPath + "/" + *iter);
            Decision d(path);
            decisions.push_back(d);
        }

        iter++;
    }
    //Decision d1(std::string("run"), 150);
    //Decision d2(std::string("walk"), 250);
   // Decision d3(std::string("hide"), 350);
    //decisions.push_back(d1);
    //decisions.push_back(d2);
    //decisions.push_back(d3);
}
Decisions::~Decisions() {
    decisions.clear();
}
std::vector<Decision>& Decisions::get_decisions(){
    return decisions;
}

} /* namespace xai */
