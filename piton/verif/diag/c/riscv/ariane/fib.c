
// Copyright 2018 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
// Author: Michael Schaffner <schaffner@iis.ee.ethz.ch>, ETH Zurich
// Date: 26.11.2018
// Description: Simple hello world program that prints the core id.
// Also runs correctly on manycore configs.
//

#include <stdint.h>
#include <stdio.h>
#include "util.h"


__attribute__((aligned(64)))
typedef union {
  uint32_t value;
  char cache_line[64];
} token_t;

// Synchronization variables
volatile token_t tokens[PITON_NUMTILES];

__attribute__((aligned(64)))
volatile union {
  uint64_t value;
  char cache_line[64];
} fib[PITON_NUMTILES];

__attribute__((section(".iterCount")))
volatile uint64_t iterCount = 1;

void waitToken(uint64_t index, uint64_t value) {
  if (index < 0) return;
  if (index >= PITON_NUMTILES) return;
  while (tokens[index].value != value);
  return;
}

int piton_main(int coreId, unsigned nCores) {
  const uint64_t iterations = iterCount;

  if (coreId <= 1) tokens[coreId].value = 1;

  for (uint64_t n = 1; n <= iterations; ++n) {
    waitToken(coreId - 2, n);
    waitToken(coreId - 1, n);

    if (coreId == 0) printf("Starting iteration %0u\n", (uint32_t)(n-1));

    uint64_t value;
    if (coreId == 0) {
      value = 0;
    } else if (coreId == 1) {
      value = n;
    } else {
      value = fib[coreId - 2].value + fib[coreId - 1].value;
    }
    fib[coreId].value = value;
    __sync_synchronize();
    tokens[coreId].value = n;

    printf("F(%0d)*%0d is %0llu\n", coreId, n, value);

    waitToken(coreId + 1, n);
    waitToken(coreId + 2, n);
  }

  return 0;
}
