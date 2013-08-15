/*
  memclinger

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

#include "memclinger.h"


static void parse_args(int argc, char **argv);
static void print_help(void);
static void print_help_exit(int status);
static void do_memory_allocation(mem_desc_t *desc);


static mem_desc_t *mem_desc_list = NULL;
static        int  nr_mem_desc = 0;
static       bool  periodical_write = false;



static int
parse_mem_desc(const char *desc_str, mem_desc_t *desc)
{
    long int val;
    long int unit;
    char *endptr;

    val = strtol(desc_str, &endptr, 10);
    if (val == LONG_MIN || val == LONG_MAX)
    {
        return -1;
    }
    switch(*endptr)
    {
    case '\0':
        unit = 1;
        break;
    case 'k': case 'K':
        unit = 1024L;
        break;
    case 'm': case 'M':
        unit = 1024L * 1024L;
        break;
    case 'g': case 'G':
        unit = 1024L * 1024L * 1024L;
        break;
    default:
        return -1;
    }

    val *= unit;

    bzero(desc, sizeof(mem_desc_t));

    desc->size = val;
    if (desc->size >= 1024L * 1024L)
    {
        desc->strategy = STRATEGY_MMAP;
        desc->size = desc->size & (~0xfffff);
    }
    else
    {
        desc->strategy = STRATEGY_MALLOC;
    }

    return 0;
}

static void
parse_args(int argc, char **argv)
{
    int c;

    while (-1 != (c = getopt(argc, argv, "-d:w")))
    {
        switch(c)
        {
        case 'd':
            nr_mem_desc ++;
            mem_desc_list = realloc(mem_desc_list, nr_mem_desc * sizeof(mem_desc_t));

            if (parse_mem_desc(optarg, &mem_desc_list[nr_mem_desc - 1]) == -1)
            {
                printf("ERROR: invalid memory description: %s\n\n", optarg);
                print_help_exit(EXIT_FAILURE);
            }
            break;
        case 'w':
            periodical_write = true;
            break;
        default:
            print_help();
            exit(EXIT_FAILURE);
        }
    }
}

static void
print_help(void)
{
    printf("Usage: memclinger OPTION [OPTIONS ...]\n"
           "\n"
           "Options:\n"
           "  -d DESC    Memory allocation description.\n"
           "             FORMAT := SIZE\n"
           "             SIZE   := NUM UNIT\n"
           "             NUM    := [1-9] [0-9]*\n"
           "             UNIT   := ( 'k' | 'K' | 'm' | 'M' | 'g' | 'G' ) ( 'b' | 'B' )?\n"
           "  -w         Periodically write meaningless values to memory regions.\n"
        );
}

static void
print_help_exit(int status)
{
    print_help();
    exit(status);
}


static void
do_memory_allocation(mem_desc_t *desc)
{
    switch(desc->strategy)
    {
    case STRATEGY_MMAP:
        desc->ptr = mmap(NULL, desc->size,
                         PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS,
                         -1,
                         0);
    break;
    case STRATEGY_MALLOC:
        posix_memalign(&desc->ptr, 4096, desc->size);
        break;
    }

    memset(desc->ptr, 0x55, desc->size);
}

int
main(int argc, char **argv)
{
    int i;

    parse_args(argc, argv);

    if (nr_mem_desc == 0)
    {
        print_help_exit(EXIT_FAILURE);
    }

    for (i = 0; i < nr_mem_desc; i++)
    {
        mem_desc_t *desc = &mem_desc_list[i];
        printf("Memory allocation %d:\n", i);
        printf("  size   %ld byte\n", desc->size);
        printf("\n");

        do_memory_allocation(desc);
    }

    while (true)
    {
        sleep(60);

        if (! periodical_write) continue;
        for (i = 0; i < nr_mem_desc; i++)
        {
            mem_desc_t *desc = &mem_desc_list[i];
            memset(desc->ptr, 0x55, desc->size);
        }
    }

    return 0;
}
