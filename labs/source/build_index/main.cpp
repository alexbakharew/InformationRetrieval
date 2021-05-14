#include <iostream>
#include <common.h>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp> 
#include <boost/archive/binary_iarchive.hpp> 
#include <unordered_map>
#include <vector>
#include <utility>

#include <utils.hpp>
#include "engine.h"

int SearchEngine::save_index_to_file(const fs::path& path_to_idx, const dictionary& dict)
{
	std::ofstream f(path_to_idx.c_str(), std::ios::binary);
	if (f.fail()) 
		return -1;
	boost::archive::binary_oarchive oa(f);
	oa << dict;
	return 0;
}

int SearchEngine::load_index_from_file(const std::string& path_to_idx, dictionary& dict)
{
	std::ifstream f(path_to_idx.c_str(), std::ios::binary);
	if (f.fail())
		return -1;
	boost::archive::binary_iarchive ia(f);
	ia >> dict;
	return 0;
}

void SearchEngine::build_index(const fs::path& work_folder)
{
	utils::recreate_dir_safely(indexed_data_folder_path);

	dictionary dict;
	ull doc_count = 0;
	ull index_count = 0;
	for (auto& entry : boost::make_iterator_range(fs::directory_iterator(tokenized_data_folder_path), {}))
	{
		if (doc_count < FilesInIndex)
		{
			std::string filename_str = entry.path().stem().string();
			ull filename_int = std::stoi(filename_str);

			std::fstream file(entry.path().string());
			std::string term;
			while (file >> term)
			{
				auto it = dict.find(term);
				if (it == dict.end())
				{
					dict.insert(std::make_pair(term, std::vector<ull>{filename_int}));
				}
				else
				{
					if (it->second.back() != filename_int)
						it->second.push_back(filename_int);
				}
			}

			++doc_count;
		}
		else
		{
			std::string index_file = std::to_string(index_count) + ".idx";
			save_index_to_file(indexed_data_folder_path / fs::path(index_file), dict);
			dict.clear();

			++index_count;
			doc_count = 0;
		}
	}
}

std::vector<int> SearchEngine::search(const std::vector<std::string> terms)
{
	std::vector<std::vector<int>> search_result(terms.size());

	dictionary dict;
	for (auto& index_path : boost::make_iterator_range(fs::directory_iterator(indexed_data_folder_path), {}))
	{
		load_index_from_file(index_path.path().string(), dict);
		for (int i = 0; i < terms.size(); ++i)
		{
			auto it = dict.find(terms[i]);
			if (it != dict.end())
			{
				//search_result[i].reserve(search_result[i].size() + it->second.size());
				search_result[i].insert(search_result[i].end(), it->second.begin(), it->second.end());
			}
		}
	}
	
	return search_result[0];
}

int main()
{
	SearchEngine se;

	std::vector<std::string> terms = { "pharmacy" };

	auto res = se.search(terms);
	std::sort(res.begin(), res.end());

	for (auto& i : res)
	{
		std::cout << i << " ";
	}
	std::cout << std::endl;

	return 0;
}
