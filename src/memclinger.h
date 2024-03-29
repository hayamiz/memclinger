/*
  memclinger.h - memclinger header

  Copyright (C) 2013 Yuto HAYAMIZU <y.hayamizu@gmail.com>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _MEMCLINGER_H_
#define _MEMCLINGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>


typedef enum {
    STRATEGY_MALLOC,
    STRATEGY_MMAP,
} strategy_t;

typedef struct {
    size_t      size;
    strategy_t  strategy;
    void       *ptr;
} mem_desc_t;

#endif
