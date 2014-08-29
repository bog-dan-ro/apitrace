/**************************************************************************
 *
 * Copyright 2014 VMware, Inc.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#ifndef SHADOW_HPP
#define SHADOW_HPP


#include <stdlib.h>
#include <stdint.h>
#include <string.h>


uint32_t
hashBlock(const void *p);


class MemoryShadow
{
    size_t size;
    size_t nBlocks;
    const uint8_t *realPtr;
    uint32_t *hashPtr;

public:
    MemoryShadow() :
        size(0),
        realPtr(0),
        hashPtr(0)
    {
    }

    ~MemoryShadow()
    {
        free(hashPtr);
    }

    typedef void (*Callback)(const void *ptr, size_t size);

    static void zero(void *_ptr, size_t _size);

    void cover(void *ptr, size_t size, bool discard);

    void update(Callback callback) const;
};


#endif // SHADOW_HPP
