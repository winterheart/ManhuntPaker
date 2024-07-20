/* SPDX-FileCopyrightText: Copyright 2024 Azamat H. Hackimov <azamat.hackimov@gmail.com> */
/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include "manhuntpaker_version.h"
#include <memory>
#include <vector>

#include "CLI11.hpp"
#include "binio.h"
#include "crc32.h"

#define XOR_KEY 0x7f

void extract(const std::filesystem::path &input, const std::filesystem::path &output) {

  std::fstream pak_stream(input, std::ios::binary | std::ios::in);

  if (!pak_stream.is_open()) {
    std::cout << std::format("Can't open input file {} for reading!", input.string()) << std::endl;
    exit(1);
  }

  std::error_code ec;
  std::filesystem::create_directories(output, ec);

  std::cout << std::format("Unpacking {} to {}... ", input.string(), output.string()) << std::endl;

  pak_header header;
  pak_stream >> header;

  for (int32_t i = 0; i < header.count; i++) {
    pak_entry entry;
    pak_stream >> entry;
    auto tell = pak_stream.tellg();
    std::filesystem::path full_path = output / entry.pathname;
    std::filesystem::create_directories(full_path.parent_path(), ec);
    std::fstream out(full_path, std::ios::binary | std::ios::out);

    std::shared_ptr<std::vector<uint8_t>> file_content = std::make_shared<std::vector<uint8_t>>(entry.size);

    pak_stream.seekg(entry.offset, std::ios::beg);
    pak_stream.read(reinterpret_cast<char *>(file_content->data()), entry.size);

    for (auto &c : *file_content) {
      c = c ^ XOR_KEY;
    }
    out.write((char *)(file_content->data()), entry.size);
    out.close();
    pak_stream.seekg(tell, std::ios::beg);

    std::cout << "  unpacked " << entry.pathname.generic_string() << std::endl;
  }
  pak_stream.close();
  std::cout << "Done!" << std::endl;
}

void pack(const std::filesystem::path &input, const std::filesystem::path &output) {
  std::vector<pak_entry> files;

  // Creating PAK
  std::fstream pak_stream(output, std::ios::binary | std::ios::out);

  if (!pak_stream.is_open()) {
    std::cout << std::format("Can't open output file {} for writing!", output.string()) << std::endl;
    exit(1);
  }

  std::cout << std::format("Packing {} to {}... ", input.string(), output.string()) << std::endl;

  for (const auto &dir_entry : std::filesystem::recursive_directory_iterator(input)) {
    if (std::filesystem::is_regular_file(dir_entry)) {
      std::filesystem::path file_path = std::filesystem::proximate(dir_entry, input);
      pak_entry entry{};
      // Manhunt expects dot on beginning
      entry.pathname = std::filesystem::path(".") / file_path;
      entry.size = UTILS::convert_le((int32_t)std::filesystem::file_size(dir_entry));
      entry.flag = UTILS::convert_le(1);

      files.push_back(entry);
    }
  }

  pak_header header;
  header.count = (int32_t)files.size();
  pak_stream << header;

  // we'll file table later, after calculating offsets

  // header size (12) + entry size (276)
  int32_t offset = 12 + 276 * header.count;
  pak_stream.seekg(offset, std::ios::beg);

  for (auto &entry : files) {
    entry.offset = offset;

    std::fstream input_file(input / entry.pathname, std::ios::binary | std::ios::in);
    if (!input_file.is_open()) {
      std::cout << std::format("Can't open input file {} for reading!", (input / entry.pathname).string()) << std::endl;
      exit(1);
    }

    std::shared_ptr<std::vector<uint8_t>> file_content = std::make_shared<std::vector<uint8_t>>(entry.size);
    input_file.read((char *)file_content->data(), entry.size);
    input_file.close();
    entry.crc = UTILS::crc32(0, file_content);

    for (auto &c : *file_content) {
      c = c ^ XOR_KEY;
    }

    pak_stream.write((char *)file_content->data(), entry.size);
    std::cout << "  adding " << entry.pathname.generic_string() << std::endl;
    offset += entry.size;
  }

  // Fill file table
  pak_stream.seekg(12, std::ios::beg);
  for (const auto &entry : files) {
    pak_stream << entry;
  }

  pak_stream.close();

  std::cout << "Done!" << std::endl;
}

int main(int argc, char *argv[]) {

  std::filesystem::path in_file;
  std::filesystem::path out_file;

  CLI::App app{"ManhuntPaker - pack/extract PAK files from Rockstar's Manhunt PC game"};
  app.set_version_flag("-v", MANHUNTPAKER_VERSION);
  argv = app.ensure_utf8(argv);
  std::cout << std::format("ManhuntPaker {} https://github.com/winterheart/ManhuntPaker\n"
                           "(c) 2024 Azamat H. Hackimov <azamat.hackimov@gmail.com>\n",
                           app.version())
            << std::endl;

  auto extract_cmd =
      app.add_subcommand("extract", "Extract PAK file into directory")->callback([&]() { extract(in_file, out_file); });
  extract_cmd->add_option("input", in_file, "Input PAK file")->required()->check(CLI::ExistingFile);
  extract_cmd->add_option("output", out_file, "Output directory");

  auto decode_cmd =
      app.add_subcommand("pack", "Pack directory into PAK file")->callback([&]() { pack(in_file, out_file); });
  decode_cmd->add_option("input", in_file, "Input directory")->required()->check(CLI::ExistingDirectory);
  decode_cmd->add_option("output", out_file, "Output PAK file");

  CLI11_PARSE(app, argc, argv);

  return 0;
}
