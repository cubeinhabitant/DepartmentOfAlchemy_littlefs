/**
 * MIT License
 * 
 * Copyright (c) 2021 Jesse Peterson for Department of Alchemy
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * Inspired by ESP and mbed implementation of littlefs.
 * https://github.com/ARMmbed/mbed-os/tree/master/storage/filesystem/littlefsv2
 * 
 */

#ifndef DEPARTMENTOFALCHEMY_LFSADAPTER_H_
#define DEPARTMENTOFALCHEMY_LFSADAPTER_H_

#include <Arduino.h>
#include "lfs.h"
#include "DepartmentOfAlchemy_FlashSPI.h"
 
 /*
  * Provides interface for supporting littlefs.
  */

class DepartmentOfAlchemy_lfsAdapter {
  public:
    DepartmentOfAlchemy_lfsAdapter(DepartmentOfAlchemy_FlashSPI *flashSPI);
    DepartmentOfAlchemy_lfsAdapter(DepartmentOfAlchemy_FlashSPI &flashSPI);

    bool begin();

    struct lfs_config getConfig();

    // mbed BlockDevice-like methods were uses in their implementation
    // given the littlefs callback methods I just implemented method
    // methods that had the same parameters since it may be more efficient
    // to use the littlefs callack data directly instead of converting
    // to the mben BlockDevice-like methods
    // https://github.com/ARMmbed/mbed-os/blob/master/storage/blockdevice/include/blockdevice/BlockDevice.h
    
    int read(lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
    
    int program(lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);

    int erase(lfs_block_t block);
    
  private:
    DepartmentOfAlchemy_FlashSPI *_flashSPI;
    struct lfs_config _config;
};

#endif /* DEPARTMENTOFALCHEMY_LFSADAPTER_H_ */
