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

__attribute__((section(".iterCount")))
uint64_t iterCount = 2;

int piton_main(unsigned coreId, unsigned nCores) {
  // synchronize with other cores and wait until it is this core's turn
  volatile int* const selfTokenp= &tokens[coreId].value;
  volatile int* const nextTokenp = &tokens[(coreId + 1) % PITON_NUMTILES].value;

  if (coreId == 0) *selfTokenp = 1;

  char msg[256];
  sprintf(msg, "I have the token (%d of %d)\n", coreId, nCores);

  const uint64_t iterations = iterCount;

  for (uint64_t n = 0; n < iterations; ++n) {
    while(*selfTokenp != n + 1);
    if (coreId == 0) printf("Iteration %0u\n", (uint32_t)n);
    printf(msg);
    __sync_fetch_and_add(nextTokenp, 1);
  }

  return 0;
}
