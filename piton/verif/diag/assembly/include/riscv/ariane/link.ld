/*
Copyright 2018 ETH Zurich and University of Bologna.
Copyright and related rights are licensed under the Solderpad Hardware
License, Version 0.51 (the "License"); you may not use this file except in
compliance with the License.  You may obtain a copy of the License at
http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
or agreed to in writing, software, hardware and materials distributed under
this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.

Author: Michael Schaffner <schaffner@iis.ee.ethz.ch>, ETH Zurich
Date: 26.11.2018
Description: Basic linker script for bare metal programs running on Ariane (copied from RISCV tests).
*/

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;

  /* text: test code section */
  .text.init : {
    *(.text.init)
  }

  . = 0x80000200;
  .text.fail : {
    *(.text.fail)
  }
  . = 0x80000220;
  .text.pass : {
    *(.text.pass)
  }

  . = 0x80000400;
  .text.madputc_table : {
    *(.text.madputc_table)
  }

  . = 0x80000680;
  .data.iterCount : {
    *(.data.nCores)
    *(.iterCount)
  }

  . = 0x80000700;
  .text : {
    *(.text)
  }

  /* data segment */
  .data : {
    *(.data)
  }

  .sdata : {
    __global_pointer$ = . + 0x800;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  }

  /* bss segment */
  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  }

  .bss : {
    *(.bss)
  }

  /* thread-local data segment */
  .tdata : {
    _tls_data = .;
    *(.tdata.begin)
    *(.tdata)
    *(.tdata.end)
  }

  .tbss : {
    *(.tbss)
    *(.tbss.end)
  }

  /* End of uninitalized data segement */
  _end : {
    _end = .;
  }
}

