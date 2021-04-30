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

namespace fs = boost::filesystem;
using ull = unsigned long long;

constexpr ull FilesInIndex = 100;

struct dictionary
{
	std::unordered_map<std::string, std::vector<ull>> umap;

	auto find(const std::string& s)
	{
		return umap.find(s);
	}

	auto insert(const std::pair<std::string, std::vector<ull>>& val)
	{
		return umap.insert(val);
	}

	auto end()
	{
		return umap.end();
	}

	auto clear()
	{
		return umap.clear();
	}

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & umap;
	}

};

int save_index_to_file(const fs::path& path_to_file, const dictionary& dict)
{
	std::ofstream f(path_to_file.c_str(), std::ios::binary);
	if (f.fail()) 
		return -1;
	boost::archive::binary_oarchive oa(f);
	oa << dict;
	return 0;
}

int load_index_from_file(const std::string& filename, dictionary& dict)
{
	return 1;
}

int main()
{
	fs::path work_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13)";

	fs::path tokenized_data_folder_path = work_folder / tokenized_data_folder;
	fs::path indexed_data_folder_path = work_folder / indexed_data_folder;

	recreate_dir_safely(indexed_data_folder_path);

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
					if(it->second.back() != filename_int)
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

	return 0;
}
