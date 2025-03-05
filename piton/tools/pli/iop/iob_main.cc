// Modified by Princeton University on June 9th, 2015
// ========== Copyright Header Begin ==========================================
//
// OpenSPARC T1 Processor File: iob_main.cc
// Copyright (c) 2006 Sun Microsystems, Inc.  All Rights Reserved.
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES.
//
// The above named program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License version 2 as published by the Free Software Foundation.
//
// The above named program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this work; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// ========== Copyright Header End ============================================

#include <cassert>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <limits>
#include <memory>
#include <unordered_map>

static constexpr size_t PAGE_BITS = 20;
static constexpr size_t PAGE_SIZE = 1 << PAGE_BITS;
static constexpr size_t PAGE_MASK = PAGE_SIZE - 1;
static constexpr size_t PAGE_WORDS = PAGE_SIZE / sizeof(uint64_t);

static class Memory {
  std::unordered_map<uint64_t, std::unique_ptr<uint64_t[]>> m_pages;
  uint64_t* m_pagep;
  uint64_t m_page_index;

  uint64_t* wordp(const uint64_t address) {
      assert((address % sizeof(uint64_t)) == 0);

      const uint64_t page_index = address >> PAGE_BITS;
      const uint64_t page_offset = address & PAGE_MASK;

      if (m_page_index != page_index) {
        const auto pair = m_pages.emplace(page_index, nullptr);
        if (pair.second) {
          uint64_t* const newp = new uint64_t[PAGE_WORDS];
          std::memset(newp, 0, PAGE_SIZE);
          pair.first->second.reset(newp);
        }
        m_pagep = pair.first->second.get();
        m_page_index = page_index;
      }

      return m_pagep + (page_offset / sizeof(uint64_t));
  }

public:
  Memory() { clear(); }

  void clear() {
    m_pages.clear();
    m_pagep = nullptr;
    m_page_index = std::numeric_limits<uint64_t>::max();
  }

  void load(const char* fileName, uint64_t address) {
    std::ifstream ifs(fileName);
    if (!ifs) {
      printf("Error: cannot open '%s'\n", fileName);
      std::abort();
    }

    uint64_t data = 0;
    uint32_t idx = 0;

    while (true) {
      const int byte = ifs.get();
      if (byte == EOF) break;
      data = data << 8 | (byte & 0xff);
      idx++;
      if (idx == 8) {
        idx = 0;
        write(address, data);
        address += 8;
      }
    }
  }

  uint64_t read(uint64_t address) { return *wordp(address); }

  void write(uint64_t address, uint64_t data) { *wordp(address) = data; }
} s_mem;


extern "C" void load_memory(const char *fileName, uint64_t address) {
  s_mem.load(fileName, address);
}

extern "C" uint64_t read_word(uint64_t address) {
  return s_mem.read(address);
}

extern "C" void write_word(uint64_t address, uint64_t data) {
  return s_mem.write(address, data);
}

