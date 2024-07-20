# ManhuntPaker - a tool extracting/packing PAK files from Rockstar's Manhunt game

## Usage

```shell
# Extracting
manhuntpaker extract ManHunt.pak output_directory
# Packing
manhuntribber pack output_directory ManHunt.new.pak
```

## Compilation

Project requires any modern C++20 compiler (gcc 10, clang 10, msvc 2019) and
cmake.

```shell
cmake -B build
cmake --build build
```

## File format

The file is just container (little-endian) with header, table of files and files itself:

| Entry          | Size                |
|----------------|---------------------|
| header         | 12 bytes            |
| table of files | 176 bytes per entry |
| files          | encrypted with XOR  |

Header is:

| Entry            | Type     | Value             |
|------------------|----------|-------------------|
| Magic            | 4 bytes  | "MHPK"            |
| Version          | uint32_t | Always is 0x20000 |
| Count of entries | uint32_t | Number of files   |

Table of files:

| Entry            | Type       | Value                           |
|------------------|------------|---------------------------------|
| Filename         | char [260] | May contain directories in path |
| Size             | uint32_t   | Size of file                    |
| Offset           | uint32_t   | Offset to begining of file      |
| Flag             | uint32_t   | Unknown, always "1"             |
| CRC32            | uint32_t   | CRC32 checksum of file.         |

Files:

Just bunch of files encrypted by XOR `0x7f`.

## License

Project licensed under LGPL-2.1 or later license. See LICENSE file for more info.

ManhuntRIBber
Copyright (C) 2024  Azamat H. Hackimov

This library is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation; either version 2.1 of the License, or (at your option)
any later version.

This library is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
details.

You should have received a copy of the GNU Lesser General Public License along
with this library; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

----

Project uses CLI11 library under a 3-Clause BSD license. See CLI11.hpp for
additional info.
