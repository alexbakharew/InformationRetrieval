#pragma once
#include <string>
#include <algorithm>
#include <boost/filesystem.hpp>
#include <exception>

namespace fs = boost::filesystem;

bool is_digit(const std::string& str)
{
	return std::all_of(str.begin(), str.end(), ::isdigit);
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