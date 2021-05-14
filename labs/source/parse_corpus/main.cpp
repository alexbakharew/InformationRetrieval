#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <string>
#include <exception>
#include <iostream>
#include <unordered_set>
#include <common.h>
#include <utils.hpp>

namespace pt = boost::property_tree;
namespace fs = boost::filesystem;

void extract_text(const fs::path& input_file_path, const fs::path& output_file_path)
{
	if (!fs::exists(input_file_path))
	{
		return;
	}

	std::fstream output_file(output_file_path.string(), std::ios::out);
	if (!output_file.is_open())
	{
		std::cerr << "Failed to open file: " << output_file_path << std::endl;
	}

	try
	{
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(input_file_path.string(), pt);
		std::unordered_set<std::string> sections;

		auto title = pt.get_child("metadata.title"); // Title of the article
		output_file << title.data() << "\n";

		bool is_abstract_printed = false;
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v, pt.get_child("abstract"))
		{
			if (!is_abstract_printed)
			{
				output_file << "Abstract\n";
				is_abstract_printed = true;
			}
			assert(v.first.empty()); // array elements have no names
			output_file << v.second.get_child("text").data() << "\n";
		}

		BOOST_FOREACH(boost::property_tree::ptree::value_type & v, pt.get_child("body_text"))
		{
			assert(v.first.empty()); // array elements have no names

			std::string curr_section = v.second.get_child("section").data();
			if (sections.find(curr_section) == sections.end()) // do not duplicate sections name in parsed data
			{
				output_file << curr_section << "\n";
				sections.insert(curr_section);
			}

			output_file << v.second.get_child("text").data() << "\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return;
}

void parse_files_in_folder(const fs::path& source_path, const fs::path& dest_path)
{
	if (!fs::is_directory(source_path) || !fs::is_directory(dest_path))
	{
		return;
	}

	for (auto& entry : boost::make_iterator_range(fs::directory_iterator(source_path), {}))
	{
		std::string filename = entry.path().stem().string();
		extract_text(entry.path(), dest_path / (filename + ".txt"));
	}

	return;
}

int main()
{
	fs::path work_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13)";
	fs::path raw_data_folder_path = work_folder / raw_data_folder;
	fs::path parsed_data_folder_path = work_folder / parsed_data_folder;

	std::vector<fs::path> folders_to_process;

	// In our first step, all unparsed text is stored in multiple folders
	// In the end, we'll have one-level folder "parsed_data" with parsed documents
	for (auto& entry : boost::make_iterator_range(fs::directory_iterator(raw_data_folder_path), {}))
	{
		if (fs::is_directory(entry))
		{
			folders_to_process.push_back(entry.path());
		}
	}

	utils::recreate_dir_safely(parsed_data_folder_path);

	for(auto& path : folders_to_process)
		parse_files_in_folder(path, parsed_data_folder_path);

	return 0;
}

