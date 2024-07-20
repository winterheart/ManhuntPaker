/* SPDX-FileCopyrightText: Copyright 2024 Azamat H. Hackimov <azamat.hackimov@gmail.com> */
/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

#include <array>
#include <cstdint>
#include <fstream>

#include "byteswap.h"

#define MAX_PATH 260

struct pak_header {
  char MAGIC[4] = {'M', 'H', 'P', 'K'};
  uint32_t version = UTILS::convert_le(0x20000);
  uint32_t count = 0;
};

struct pak_entry {
  std::filesystem::path pathname; // limited to MAX_PATH (260)
  uint32_t size;
  uint32_t offset;
  uint32_t flag;
  uint32_t crc;
};

std::ostream &operator<<(std::ostream &out, pak_header &header) {
  out.write(header.MAGIC, 4);
  UTILS::bin_write(out, header.version);
  UTILS::bin_write(out, header.count);

  return out;
}

std::istream &operator>>(std::istream &in, pak_header &header) {
  in.read(header.MAGIC, 4);
  UTILS::bin_read(in, header.version);
  UTILS::bin_read(in, header.count);

  return in;
}

std::ostream &operator<<(std::ostream &out, const pak_entry &entry) {
  std::array<char, MAX_PATH> path{};

  // Manhunt expects dot on beginning, with slashes as separators
  std::string tmp = entry.pathname.generic_string();

  std::copy(tmp.begin(), tmp.end(), path.begin());

  out.write(path.data(), path.size());

  UTILS::bin_write(out, entry.size);
  UTILS::bin_write(out, entry.offset);
  UTILS::bin_write(out, entry.flag);
  UTILS::bin_write(out, entry.crc);

  return out;
}

std::istream &operator>>(std::istream &in, pak_entry &entry) {

  char path[260];

  in.read(path, MAX_PATH);
  entry.pathname = std::filesystem::path(path);
  UTILS::bin_read(in, entry.size);
  UTILS::bin_read(in, entry.offset);
  UTILS::bin_read(in, entry.flag);
  UTILS::bin_read(in, entry.crc);

  return in;
}