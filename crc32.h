/* SPDX-FileCopyrightText: Copyright 2024 Azamat H. Hackimov <azamat.hackimov@gmail.com> */
/* SPDX-License-Identifier: LGPL-2.1-or-later */

#pragma once

namespace UTILS {

uint32_t crc32(uint32_t CRC, const std::shared_ptr<std::vector<uint8_t>> &Data);

}