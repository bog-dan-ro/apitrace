
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


#include "shadow.hpp"

#include <assert.h>
#include <string.h>

#include <algorithm>


// We must reset the data on discard, otherwise the old data could match just
// by chance.
//
// XXX: if the appplication writes 0xCDCDCDCD at the start or the end of the
// buffer range, we'll fail to detect.  The only way to be 100% sure things
// won't fall through would be to setup memory traps.
void MemoryShadow::zero(void *_ptr, size_t _size)
{
    memset(_ptr, 0xCD, _size);
}


void MemoryShadow::cover(void *_ptr, size_t _size, bool _discard)
{
    if (_size != size) {
        shadowPtr = (uint8_t *)realloc(shadowPtr, _size);
        size = _size;
    }

    realPtr = (const uint8_t *)_ptr;

    if (_discard) {
        zero(_ptr, size);
        zero(shadowPtr, size);
    } else {
        memcpy(shadowPtr, realPtr, size);
    }
}


template< class T >
static inline T *
lAlignPtr(T *p, uintptr_t alignment)
{
    return reinterpret_cast<T *>(reinterpret_cast<uintptr_t>(p) & ~(alignment - 1));
}


template< class T >
static inline T *
rAlignPtr(T *p, uintptr_t alignment)
{
    return reinterpret_cast<T *>((reinterpret_cast<uintptr_t>(p) + alignment - 1) & ~(alignment - 1));
}


void MemoryShadow::update(Callback callback) const
{
    const uint8_t *realStart   = realPtr;
    const uint8_t *realStop    = realPtr   + size;
    const uint8_t *shadowStart = shadowPtr;
    const uint8_t *shadowStop  = shadowPtr + size;

    assert(realStart   <= realStop);
    assert(shadowStart <= shadowStop);

    // Shrink the start to skip unchanged bytes
    while (realStart < realStop && *realStart == *shadowStart) {
        ++realStart;
        ++shadowStart;
    }

    // Shrink the stop to skip unchanged bytes
    while (realStart < realStop && realStop[-1] == shadowStop[-1]) {
        --realStop;
        --shadowStop;
    }

    assert(realStart   <= realStop);
    assert(shadowStart <= shadowStop);

    /*
     * TODO: Consider 16 bytes alignment
     * See also http://msdn.microsoft.com/en-gb/library/windows/desktop/bb205132.aspx
     */
    const uintptr_t alignment = 4;

    realStart = std::max(lAlignPtr(realStart, alignment), realPtr);
    realStop  = std::min(rAlignPtr(realStop,  alignment), realPtr + size);

    assert(realStart   <= realStop);
    assert(shadowStart <= shadowStop);

    // Update the rest
    if (realStart < realStop) {
        callback(realStart, realStop - realStart);
    }
}
