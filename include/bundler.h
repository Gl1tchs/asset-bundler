// MIT License
//
// Copyright (c) 2024 Berke Umut Biricik
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// You first need to add this before including the bundler in order it
// to work. This should only be defined if you are using the functions
// if you need definitions you don't need to define it.
//
// #define ASSET_BUNDLER_IMPLEMENTATION
//

#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <vector>

#define MAKE_VERSION(major, minor, patch)                                      \
	(((major) << 16) | ((minor) << 8) | (patch))

namespace fs = std::filesystem;

namespace bundler {

typedef uint64_t UID;

struct Asset {
	UID uid;
	std::string rel_path;
};

typedef std::vector<Asset> AssetPack;

struct AssetIndex {
	UID uid;
	uint32_t start;
	uint32_t size;
	// 16 bytes per index
};

extern uint32_t get_bundler_version();

extern std::optional<AssetPack> read_asset_pack(const fs::path& path);

/*
 *  head:
 *      file version: 4 bytes,
 *      asset index count: 4 bytes,
 *      data start position: 4 bytes,
 *  asset index tree:
 *      id: 8 bytes,
 *      start: 4 bytes,
 *      size: 4 bytes,
 *  data tree
 */
extern int write_data(const AssetPack& pack, fs::path pack_path);

} //namespace bundler

#ifdef ASSET_BUNDLER_IMPLEMENTATION

#include <fstream>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace bundler {

inline void print_parsing_error_message(
		const fs::path& path, const uint32_t line_number) {
	std::cerr << "Error: parsing file " << path.filename()
			  << " at line: " << line_number << ", resulted with an error.\n";
}

#ifdef _WIN32
inline bool get_file_size(const std::string& file_path, uint32_t& file_size) {
	WIN32_FILE_ATTRIBUTE_DATA file_attrs;
	if (!GetFileAttributesExA(
				file_path.c_str(), GetFileExInfoStandard, &file_attrs)) {
		return false;
	}
	file_size = static_cast<uint32_t>(file_attrs.nFileSizeLow);
	return true;
}
#else
inline bool get_file_size(const std::string& file_path, uint32_t& file_size) {
	struct stat st {};
	if (stat(file_path.c_str(), &st) != 0) {
		return false;
	}
	file_size = static_cast<uint32_t>(st.st_size);
	return true;
}
#endif

extern uint32_t get_bundler_version() { return MAKE_VERSION(0, 1, 0); }

extern std::optional<AssetPack> read_asset_pack(const fs::path& path) {
	std::ifstream file(path);

	if (!file.is_open()) {
		return {};
	}

	AssetPack pack{};

	const std::string delimeter = " ";

	uint32_t line_number = 1;
	std::string line_buffer;

	while (std::getline(file, line_buffer)) {
		size_t pos = line_buffer.find(delimeter);
		if (pos == std::string::npos) {
			print_parsing_error_message(path, line_number);
			std::cerr << "Error: cannot find space ( ) delimeter!\n";
			return {};
		}

		Asset asset;

		// uid
		std::string token = line_buffer.substr(0, pos);

		try {
			asset.uid = std::stoull(token);
		} catch (const std::exception& e) {
			print_parsing_error_message(path, line_number);
			std::cerr << "Error: provided UID is not valid!\n";
			return {};
		}

		// path
		token = line_buffer.substr(
				pos + delimeter.length(), line_buffer.length());

		asset.rel_path = token;

		line_number++;

		pack.push_back(asset);
	}

	return pack;
}

extern int write_data(const AssetPack& pack, fs::path pack_path) {
	const fs::path pack_dir = pack_path.parent_path();

	std::ofstream out(pack_path.replace_extension(".apkg.bin"),
			std::ios::binary | std::ios::out);

	// write version
	const uint32_t version = get_bundler_version();
	out.write(reinterpret_cast<const char*>(&version), 4);

	// write asset indices size
	const uint32_t indices_count = pack.size();
	out.write(reinterpret_cast<const char*>(&indices_count), 4);

	// write data start pos
	const uint32_t data_start_pos = 4 // version
			+ 4 // indices count
			+ 4 // data start pos
			+ indices_count * sizeof(AssetIndex); // indices
	out.write(reinterpret_cast<const char*>(&data_start_pos), 4);

	// write index tree
	uint32_t end_pos = 0;
    uint32_t line_number = 1;

	for (const auto& asset : pack) {
		AssetIndex index;
		index.uid = asset.uid;
		index.start = end_pos;
		if (!get_file_size(pack_dir / asset.rel_path, index.size)) {
			std::cerr << "Error: unable to get file size from file: "
					  << pack_dir / asset.rel_path << ", at line: " << line_number << "\n";
			return 1;
		}

		// write asset index
		out.write(reinterpret_cast<const char*>(&index), sizeof(AssetIndex));

		// increase end position
		end_pos += index.size;

        line_number++;
	}

	// write data
	for (const auto& asset : pack) {
		std::ifstream asset_file(pack_dir / asset.rel_path, std::ios::binary);
		if (!asset_file.is_open()) {
			std::cerr << "Error: unable to read data from file: "
					  << pack_dir / asset.rel_path << "\n";
			return 1;
		}

		// write bytes
		char byte;
		while (asset_file.get(byte)) {
			out.write(&byte, sizeof(char));
		}
	}

	return 0;
}

} //namespace bundler

#endif
