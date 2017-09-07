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
#include <dirent.h>
#include <vector>
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
std::vector<std::string> FileUtils::list_files(const std::string &s){
    DIR *dir;
    struct dirent *ent;
    std::vector<std::string> filenames;
    if ((dir = opendir (s.c_str())) != NULL) {
      /* print all the files and directories within directory */
      while ((ent = readdir (dir)) != NULL) {
          std::string filename(ent->d_name);
          filenames.push_back(filename);
          printf ("%s\n", ent->d_name);
      }
      closedir (dir);
    } else {
      /* could not open directory */
      throw domain_error("problem listing files from  " + s);

    }
    return filenames;
}
bool FileUtils::has_filetype(const std::string &s, const std::string &type){
    if (type.size() > s.size()) return false;
    return std::equal(type.rbegin(), type.rend(), s.rbegin());
}
std::string FileUtils::get_file_root(const std::string &s){
    std::string::const_iterator iter = s.begin();
    while (*iter != '.' && iter < s.end()){
        iter++;
    }
    std::string root;
    copy(s.begin(), iter, back_inserter(root));
    return root;
}




