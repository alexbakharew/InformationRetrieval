#pragma once
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <exception>
#include <sys/stat.h>

namespace fs = boost::filesystem;

namespace utils
{

bool is_digit(const std::string& str)
{
	bool res;
	try
	{
		res = std::all_of(str.begin(), str.end(), ::isdigit);
	}
	catch (...)
	{
		std::cout << "skip " << str << std::endl;
	}
	return res;
}

void recreate_dir_safely(const fs::path& path)
{
	try
	{
		if (fs::exists(path))
		{
			fs::remove_all(path);
		}

		fs::create_directory(path);
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		exit(-1);
	}

	return;
}

long GetFileSize(std::string filename)
{
	struct stat stat_buf;
	int rc = stat(filename.c_str(), &stat_buf);
	return rc == 0 ? stat_buf.st_size : -1;
}

} // utils