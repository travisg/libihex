/*
 * Copyright (c) 2013 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <cstdio>
#include <cstdlib>
#include <getopt.h>
#include <string>
#include <iostream>
#include <fstream>

#include "ihex.h"

using namespace std;

static size_t fileoffset = 0;
static string inputfile;
static string outputfile;
static ofstream out;

static void usage(int, char **argv) {
    printf("usage: %s <input file> <output file>\n", argv[0]);

    exit(1);
}
void ihexcallback(const uint8_t *ptr, size_t offset, size_t len) {
    //printf("callback: offset %#zx, len %zu\n", offset, len);

    // apply the file offset
    if (fileoffset > offset) {
        fprintf(stderr, "error: offset in hex file out of range of file offset, o %#zx to %#zx\n", offset, fileoffset);
        exit(1);
    }

    offset -= fileoffset;

    out.seekp(offset);
    out.write((const char *)ptr, len);
}

int main(int argc, char **argv) {
    for (;;) {
        int c;
        int option_index = 0;

        static struct option long_options[] = {
            {"offset", 1, 0, 'o'},
            {"help", 0, 0, 'h'},
            {0, 0, 0, 0},
        };

        c = getopt_long(argc, argv, "ho:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 'o':
                fileoffset = strtoul(optarg, NULL, 0);
                printf("offset %#zx\n", fileoffset);
                break;
            case 'h':
            default:
                usage(argc, argv);
                break;
        }
    }

    if (argc - optind < 2) {
        printf("not enough arguments\n");
        usage(argc, argv);
    }

    inputfile = argv[optind++];
    outputfile = argv[optind++];

    printf("input file %s\n", inputfile.c_str());
    printf("output file %s\n", outputfile.c_str());

    iHex hex;
    if (hex.Open(inputfile) < 0) {
        fprintf(stderr, "error opening input file\n");
        return 1;
    }

    out.open(outputfile.c_str(), ios::out|ios::trunc|ios::binary);
    if (!out.is_open()) {
        fprintf(stderr, "error opening output file\n");
        return 1;
    }

    hex.SetCallback(&ihexcallback);
    hex.Parse();

    hex.Close();

    out.close();

    return 0;
}

// vim: ts=4:sw=4:expandtab:
