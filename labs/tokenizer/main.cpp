#include <iostream>
#include <regex>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <fstream>

namespace fs = boost::filesystem;

void tokenize_text(const fs::path& input, const fs::path& output)
{
	std::fstream input_file(input.string(), std::ios::in);
	std::fstream output_file(output.string(), std::ios::out);

	std::string curr_str;
	const std::regex re(R"([\s|,|.|;|:]+)");

	while (input_file >> curr_str)
	{
		std::sregex_token_iterator it{curr_str.begin(), curr_str.end(), re, -1 };
		std::vector<std::string> tokenized{ it, {} };

		// Additional check to remove empty strings
		tokenized.erase(std::remove_if(tokenized.begin(), tokenized.end(),[](std::string const& s) {
			return s.size() == 0;}),
		tokenized.end());

		for (auto& token : tokenized)
		{
			output_file << token;
		}
		output_file << std::endl;
	}

}

void make_tokenization(const fs::path& source, const fs::path& destination)
{
	for (auto& file : boost::make_iterator_range(fs::directory_iterator(source), {}))
	{
		std::string filename = file.path().stem().string();
		tokenize_text(file, destination / (filename + ".txt"));
	}
}

int main()
{
	fs::path parsed_data = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13\parsed_data\biorxiv_medrxiv)";
	if (!fs::exists(parsed_data))
	{
		std::cerr << "No folder with parsed data..." << std::endl;
		return -1;
	}

	fs::path tokenized_data_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13\tokenized_data)";
	if (!fs::exists(tokenized_data_folder))
	{
		fs::remove_all(tokenized_data_folder);
		fs::create_directory(tokenized_data_folder);
	}
	
	fs::path tokenized_data_output = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13\tokenized_data\biorxiv_medrxiv)";
	if (fs::exists(tokenized_data_output))
	{
		fs::remove_all(tokenized_data_output);
	}

	fs::create_directory(tokenized_data_output);


	make_tokenization(parsed_data, tokenized_data_output);

	return 0;
}
