#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#include <string>
#include <stdexcept>

class FileUtils{
public:
	static bool is_valid_dir(const std::string &s);
	static bool ensure_directory_exists(const std::string &s);
	static bool clean_directory(const std::string &s);
};
#endif
