#include <iostream>
#include <regex>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>
#include <algorithm>

#include <utils.hpp>
#include <common.h>
#include "stop_words.h"

namespace fs = boost::filesystem;

bool is_token_correct(const std::string& low_str)
{
	if (stop_words.find(low_str) != stop_words.end()) // stop words
		return false;

	if (is_digit(low_str))
		return false;

	return true;
}

void tokenize_text(const fs::path& input, const fs::path& output)
{
	std::fstream input_file(input.string(), std::ios::in);
	std::fstream output_file(output.string(), std::ios::out);
	// for read-write speed-up: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring

	std::string curr_str;
	const std::regex re(R"([\s|,|.|;|:|(|)|<|>|=|+|\/|[|\]|'|%|-|°|*|?|!|@|\"]+)");
	
	while (std::getline(input_file, curr_str))
	{
		std::sregex_token_iterator it{curr_str.begin(), curr_str.end(), re, -1 };
		std::vector<std::string> tokenized{ it, {} };

		// Additional check to remove empty strings and one-char strings
		tokenized.erase(std::remove_if(tokenized.begin(), tokenized.end(),[](std::string const& s) {
			return s.size() == 0 || s.size() == 1;}),
		tokenized.end());

		for (auto& token : tokenized)
		{
			std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) { return std::tolower(c); });

			if (is_token_correct(token))
			{
				output_file << token << "\n";
			}
		}
	}

	return;
}

void make_tokenization(const fs::path& source, const fs::path& destination)
{
	unsigned long long filename_count = 0;
	for (auto& file : boost::make_iterator_range(fs::directory_iterator(source), {}))
	{
		tokenize_text(file, destination / (std::to_string(filename_count) + ".txt"));
		++filename_count;
	}

	return;
}

int main()
{
	fs::path work_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13)";
	fs::path parsed_data_folder_path = work_folder / parsed_data_folder;
	fs::path tokenized_folder_path = work_folder / tokenized_data_folder;

	recreate_dir_safely(tokenized_folder_path);

	make_tokenization(parsed_data_folder_path, tokenized_folder_path);

	return 0;
}
