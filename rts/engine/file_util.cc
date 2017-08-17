/**
* Copyright (c) 2017-present, Facebook, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-style license found in the
* LICENSE file in the root directory of this source tree. An additional grant
* of patent rights can be found in the PATENTS file in the same directory.
*/

#include "file_util.h"
#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
//#include "/usr/include/boost/filesystem.hpp"

using namespace std;

bool FileUtils::is_valid_dir(const std::string &s) {
    //return boost::filesystem::is_directory(s);
    struct stat info;

    if( stat( s.c_str(), &info ) != 0 ){
        cout << "cannot access "<< s << endl;
        return false;
    }
    else if( info.st_mode & S_IFDIR ){
        cout << s << " is a directory" << endl;
        return true;
    }
    else {
        cout << s << "is no directory" << endl;
        return false;
    }
}

bool FileUtils::ensure_directory_exists(const std::string &s){
	if (FileUtils::is_valid_dir(s)){
		return true;
	}
	cout << "creating directory " << s << endl;
    try {
        //return boost::filesystem::create_directory(s);
        string command = "mkdir -p " + s;
        int result = system(command.c_str());
        if (result == 0){
            return true;
        }
        return false;
    } catch (exception &e) {
        cout << "had problem creating directory" << endl;
        cout << e.what() << endl;
        return false;
    }

}
bool FileUtils::clean_directory(const std::string &s){
	cout << "cleaning directory " << s << endl;
	try {
		string command = "rm -r " + s;
		int result = system(command.c_str());
		return result;
	}
	catch (exception &e){
		cout << "had problem cleaning directory"<< endl;
		cout << e.what() << endl;
		return false;
	}
}
