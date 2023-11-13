/***************************************************************************************
* Copyright (c) 2020-2021 Institute of Computing Technology, Chinese Academy of Sciences
* Copyright (c) 2020-2021 Peng Cheng Laboratory
*
* XiangShan is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "compress.h"

double calcTime(timeval s, timeval e) {
  double sec, usec;
  sec = e.tv_sec - s.tv_sec;
  usec = e.tv_usec - s.tv_usec;
  return 1000*sec + usec/1000.0;
}

void nonzero_large_memcpy(const void* __restrict dest, const void* __restrict src, size_t n) {
  uint64_t *_dest = (uint64_t *)dest;
  uint64_t *_src  = (uint64_t *)src;
  while (n >= sizeof(uint64_t)) {
    if (*_src != 0) {
      *_dest = *_src;
    }
    _dest++;
    _src++;
    n -= sizeof(uint64_t);
  }
  if (n > 0) {
    uint8_t *dest8 = (uint8_t *)_dest;
    uint8_t *src8  = (uint8_t *)_src;
    while (n > 0) {
      *dest8 = *src8;
      dest8++;
      src8++;
      n--;
    }
  }
}
