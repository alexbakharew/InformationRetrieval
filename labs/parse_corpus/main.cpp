#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>

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

		auto title = pt.get_child("metadata.title"); // Title of the article
		output_file << title.data() << std::endl;

		// Todo: It is possible to be no Abstract section
		output_file << "Abstract" << std::endl;
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v, pt.get_child("abstract"))
		{
			assert(v.first.empty()); // array elements have no names
			output_file << v.second.get_child("text").data() << std::endl;
		}

		// Todo: Add sections from text
		output_file << "Text" << std::endl;
		BOOST_FOREACH(boost::property_tree::ptree::value_type & v, pt.get_child("body_text"))
		{
			assert(v.first.empty()); // array elements have no names
			output_file << v.second.get_child("text").data() << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return;
}

void parse_files_in_folder(const fs::path& source_path)
{
	if (!fs::is_directory(source_path))
	{
		return;
	}

	fs::path parent_folder = source_path.parent_path();
	std::string source_folder_name = source_path.filename().string();
	fs::path folder_with_parsed_data = parent_folder / "parsed_data";

	if (!fs::exists(folder_with_parsed_data))
	{
		fs::create_directory(folder_with_parsed_data);
	}

	fs::path dest_path = folder_with_parsed_data / source_folder_name;
	if (fs::exists(dest_path))
	{
		fs::remove_all(dest_path);
	}
	fs::create_directory(dest_path);

	for (auto& entry : boost::make_iterator_range(fs::directory_iterator(source_path), {}))
	{
		std::string filename = entry.path().stem().string();
		extract_text(entry.path(), dest_path / (filename + ".txt"));
	}

	return;
}

int main()
{
	fs::path input_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13)";

	std::vector<fs::path> folders_to_process;

	for (auto& entry : boost::make_iterator_range(fs::directory_iterator(input_folder), {}))
	{
		if (fs::is_directory(entry))
		{
			folders_to_process.push_back(entry.path());
		}
	}

	for(auto& path : folders_to_process)
		parse_files_in_folder(path);

	return 0;
}

