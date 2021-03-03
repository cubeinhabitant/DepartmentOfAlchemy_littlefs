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
 * Inspired by Adafruit SPIFlash library:
 * https://github.com/adafruit/Adafruit_SPIFlash
 * Check out their library for a full featured implementation.
 * 
 * This is just the basics needed to get the littlefs filesystem to operate.
 */

#ifndef DEPARTMENTOFALCHEMY_FLASHSPI_H_
#define DEPARTMENTOFALCHEMY_FLASHSPI_H_

#include <Arduino.h>
#include <SPI.h>

#define BIT_BUSY 0x01
#define BIT_WEL  0x02
#define BIT_SUS  0x80

enum {
  DOA_FLASH_CMD_FAST_READ      = 0x0b,
  DOA_FLASH_CMD_READ_STATUS_1  = 0x05,
  DOA_FLASH_CMD_READ_STATUS_2  = 0x35,
  DOA_FLASH_CMD_READ_STATUS_3  = 0x15,
  DOA_FLASH_CMD_READ_JEDEC_ID  = 0x9F,
  DOA_FLASH_CMD_ENABLE_RESET   = 0x66,
  DOA_FLASH_CMD_RESET          = 0x99,
  DOA_FLASH_CMD_WRITE_ENABLE   = 0x06,
  DOA_FLASH_CMD_WRITE_DISABLE  = 0x04,
  DOA_FLASH_CMD_WRITE_STATUS_1 = 0x01,
  DOA_FLASH_CMD_PAGE_PROGRAM   = 0x02,
  DOA_FLASH_CMD_SECTOR_ERASE   = 0x20,
};

enum {
  DOA_FLASH_SECTOR_SIZE = 4 * 1024, // 4K-bytes
  DOA_FLASH_PAGE_SIZE = 256,         // 256 bytes
};

class DepartmentOfAlchemy_FlashSPI {
  public:
    /**
     * @param ss Slave select pin.
     * @param spi The SPI interface to use to communicate with the flash ram device.
     * @param skipSpiBegin Set to <code>true</code> to skip calling the <code>begin</code>
     *                     method of the <code>spi</code> object. May not want to call
     *                     it if the device is a non-standard SPI device that needs to
     *                     configure its pins. Arduino may overwrite the configuration
     *                     in the <code>begin</code> method.
     */
    DepartmentOfAlchemy_FlashSPI(uint8_t ss, SPIClass *spi, bool skipSpiBegin = false);
    DepartmentOfAlchemy_FlashSPI(uint8_t ss, SPIClass &spi, bool skipSpiBegin = false);

    /**
     * Determine if the skipSpiBegin option is enabled.
     */
    bool skipSpiBegin();
    
    bool begin();

    /**
     * Get the total size, in bytes, of the flash device
     * 
     * @return The total size, in bytes, of the flash device
     */
    uint32_t size() const;

    bool runCommand(uint8_t command);
    bool readCommand(uint8_t command, uint8_t *response, uint32_t len);
    bool writeCommand(uint8_t command, uint8_t const *data, uint32_t len);
    bool eraseCommand(uint8_t command, uint32_t addr);

    bool readMemory(uint32_t addr, uint8_t *data, uint32_t len);
    bool writeMemory(uint32_t addr, uint8_t const *data, uint32_t len);



    uint8_t readStatus1();
    uint8_t readStatus2();
    uint8_t readStatus3();
    void waitUntilReady();
    bool writeEnable();
    bool writeDisable();

    /**
     * Read a buffer length of data.
     * 
     * @param address The address to start reading from.
     * @param buffer The buffer to read data to.
     * @param len The length of data to read.
     * @return The length of data read. May be 0 if no data read.
     */
    uint32_t readBuffer(uint32_t address, uint8_t *buffer, uint32_t len);
    uint32_t writeBuffer(uint32_t address, uint8_t const *buffer, uint32_t len);

    /**
     * Erase a sector (4K-bytes).
     */
    bool eraseSector(uint32_t sectorNumber);

  protected:
    // Number of bytes for address
    uint8_t _addrLen;
    
  private:
    uint8_t _ss;
    SPIClass *_spi;
    bool _skipSpiBegin;

    uint32_t _clockRd;
    uint32_t _clockWr;

    uint32_t _totalSize; // in megabytes

    // -----------
    void fillAddress(uint8_t *buf, uint32_t addr);
    
    void beginTransaction(uint32_t clockHz) {
      _spi->beginTransaction(SPISettings(clockHz, MSBFIRST, SPI_MODE0));
      digitalWrite(_ss, LOW);
    }

    void endTransaction() {
      digitalWrite(_ss, HIGH);
      _spi->endTransaction();
    }
};

#endif /* DEPARTMENTOFALCHEMY_FLASHSPI_H_ */
