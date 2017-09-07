#ifndef _FILE_UTILS_H_
#define _FILE_UTILS_H_

#include <string>
#include <stdexcept>
#include <vector>
class FileUtils{
public:
	static bool is_valid_dir(const std::string &s);
	static bool ensure_directory_exists(const std::string &s);
	static bool clean_directory(const std::string &s);
	static std::vector<std::string> list_files(const std::string &s);
	static bool has_filetype(const std::string &s, const std::string &type);
	static std::string get_file_root(const std::string &s);
};
#endif
