#include <cstring>
#include <iostream>

#define ASSET_BUNDLER_IMPLEMENTATION
#include "bundler.h"

static void print_help_message() {
	std::cout << "asset-bundler\n"
				 "usage:\n"
				 "    ./asset-bundler asset-pack.apkg\n";
}

int main(const int argc, const char* argv[]) {
	if (argc < 2) {
		print_help_message();
		return 0;
	}

	if (!strcmp(argv[1], "--help") || !strcmp(argv[1], "-h")) {
		print_help_message();
		return 0;
	}

	const fs::path asset_pack_path = argv[1];
	if (!fs::exists(asset_pack_path)) {
		std::cerr << "Asset pack file does not exists.\n";
		return 1;
	}

	const auto asset_pack_opt = bundler::read_asset_pack(asset_pack_path);
	if (!asset_pack_opt.has_value()) {
		return 1;
	}

	fs::path output_path = [=]() {
		if (argc == 3) {
			return fs::path(argv[2]);
		} else {
			auto temp_path = asset_pack_path;
			temp_path.replace_extension("apkg.bin");
			return temp_path;
		}
	}();

	bundler::AssetPack asset_pack = asset_pack_opt.value();
	int result = bundler::write_data(asset_pack, asset_pack_path, output_path);

	if (result == 0) {
		std::cout << "Assets bundled successufully!\n";
	}

	return result;
}
