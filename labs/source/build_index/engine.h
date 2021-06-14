#pragma once
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

constexpr ull FilesInIndex = 1000;

struct dictionary
{
	std::unordered_map<std::string, std::vector<ull>> umap;

	auto find(const std::string& s)
	{
		return umap.find(s);
	}

	auto insert(const std::pair<std::string, std::vector<ull>>&& val)
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
		ar& umap;
	}

};

class SearchEngine
{
public:
	std::vector<int> search(const std::vector<std::string> terms);
	void build_index(const fs::path& work_folder);

private:
	int save_index_to_file(const fs::path& path_to_idx, const dictionary& dict);
	int load_index_from_file(const std::string& path_to_idx, dictionary& dict);

private:
	const fs::path work_folder = LR"(C:\Users\Alexey\Desktop\IS\2020-03-13)";
	const fs::path tokenized_data_folder_path = work_folder / tokenized_data_folder;
	const fs::path indexed_data_folder_path = work_folder / indexed_data_folder;
};