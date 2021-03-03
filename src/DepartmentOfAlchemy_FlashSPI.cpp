
#include "DepartmentOfAlchemy_FlashSPI.h"

DepartmentOfAlchemy_FlashSPI::DepartmentOfAlchemy_FlashSPI(uint8_t ss, SPIClass *spi, bool skipSpiBegin) {
  _addrLen = 3; // default to 24 bit address
  _ss = ss;
  _spi = spi;
  _skipSpiBegin = skipSpiBegin;

  // Adafruit testing shows SAMD21 can write up to 24 Mhz but on read at 12 Mhz
  _clockRd = 12000000; // 12 Mhz
  _clockWr = 12000000; // 24 Mhz, had problems with custom SPI at 24 Mhz
}

DepartmentOfAlchemy_FlashSPI::DepartmentOfAlchemy_FlashSPI(uint8_t ss, SPIClass &spi, bool skipSpiBegin)
  : DepartmentOfAlchemy_FlashSPI(ss, &spi, skipSpiBegin) {}

bool DepartmentOfAlchemy_FlashSPI::skipSpiBegin() {
  return _skipSpiBegin;
}

bool DepartmentOfAlchemy_FlashSPI::begin() {
  pinMode(_ss, OUTPUT);
  digitalWrite(_ss, HIGH);

  if (!_skipSpiBegin) {
    _spi->begin();
  }

  uint8_t jedec[4];

  readCommand(DOA_FLASH_CMD_READ_JEDEC_ID, jedec, 4);

  if ((jedec[0] == 0xEF) &&
      (jedec[1] == 0x40) &&
      (jedec[2] == 0x17)) {
    // Winbond W25Q64JV-IQ
    _totalSize = (1UL << 23); /* 8 MB */ 
  } else {
    Serial.println("Unsupported FLASH memory");
    Serial.println(((uint32_t)jedec[0]) << 16 | jedec[1] << 8 | jedec[2], HEX);
    return false;
  }

  // Get flash into known state by performing a reset.
  // From the datasheet, the reset is a software reset. The enable reset command
  // must be followed by the reset command. It is recommended to check the BUSY
  // bit and the SUS bit before issuing the reset command sequence.

  // Wait for any write to complete
  while (readStatus1() & BIT_BUSY) {}

  // Wait for Erase/Program suspend to complete
  while (readStatus2() & BIT_SUS) {}

  runCommand(DOA_FLASH_CMD_ENABLE_RESET);
  runCommand(DOA_FLASH_CMD_RESET);

  // Once the reset command is accepted the device will take approximately 30us
  // to reset.
  delayMicroseconds(30);

  writeDisable();
  waitUntilReady();
  
  return true;
}

uint32_t DepartmentOfAlchemy_FlashSPI::size() const {
  return _totalSize;
}

//
// Basic Flash SPI commands
//

bool DepartmentOfAlchemy_FlashSPI::runCommand(uint8_t command) {
  beginTransaction(_clockRd);

  _spi->transfer(command);
  
  endTransaction();

  return true;
}

bool DepartmentOfAlchemy_FlashSPI::readCommand(uint8_t command, uint8_t *response, uint32_t len) {
  beginTransaction(_clockRd);

  _spi->transfer(command);
  while (len--) {
    *response++ = _spi->transfer(0x00);
  }

  endTransaction();

  return true;
}

bool DepartmentOfAlchemy_FlashSPI::writeCommand(uint8_t command, uint8_t const *data, uint32_t len) {
  beginTransaction(_clockWr);

  _spi->transfer(command);
  while (len--) {
    _spi->transfer(*data++);
  }

  endTransaction();

  return true;
}

bool DepartmentOfAlchemy_FlashSPI::eraseCommand(uint8_t command, uint32_t addr) {
  beginTransaction(_clockWr);

  uint8_t cmdWithAddr[5] = {command};
  fillAddress(cmdWithAddr + 1, addr); // pack address after write command

  _spi->transfer(cmdWithAddr, 1 + _addrLen);

  endTransaction();

  return true;
}

void DepartmentOfAlchemy_FlashSPI::fillAddress(uint8_t *buf, uint32_t addr) {
  // copy the address into the byte buffer
  switch (_addrLen) {
  case 3: // 24 bit address
    *buf++ = (addr >> 16) & 0xFF;
    // fall through to shift over rest of address bits

  case 2: // 16 bit address
  default:
    *buf++ = (addr >> 8) & 0xFF;
    *buf++ = addr & 0xFF;
  }
}

bool DepartmentOfAlchemy_FlashSPI::readMemory(uint32_t addr, uint8_t *data, uint32_t len) {
  beginTransaction(_clockRd);

  uint8_t cmdWithAddr[6] = {DOA_FLASH_CMD_FAST_READ};
  fillAddress(cmdWithAddr + 1, addr); // pack address after read command

  // Fast Read requires a dummy byte to enter fast read mode
  uint8_t const cmdLen = 1 + _addrLen + 1;

  _spi->transfer(cmdWithAddr, cmdLen);

  // TODO: Can potentially improve performance with DMA
  _spi->transfer(data, len);

  endTransaction();

  return true;
}

bool DepartmentOfAlchemy_FlashSPI::writeMemory(uint32_t addr, uint8_t const *data, uint32_t len) {
  beginTransaction(_clockWr);

  uint8_t cmdWithAddr[5] = {DOA_FLASH_CMD_PAGE_PROGRAM};
  fillAddress(cmdWithAddr + 1, addr); // pack address after write command

  _spi->transfer(cmdWithAddr, 1 + _addrLen);

  // TODO: Can potentially improve performance with DMA
  while (len--) {
    _spi->transfer(*data++);
  }

  endTransaction();

  return true;
}

// ----------

uint8_t DepartmentOfAlchemy_FlashSPI::readStatus1() {
  uint8_t status1;
  readCommand(DOA_FLASH_CMD_READ_STATUS_1, &status1, 1);
  return status1;
}

uint8_t DepartmentOfAlchemy_FlashSPI::readStatus2() {
  uint8_t status2;
  readCommand(DOA_FLASH_CMD_READ_STATUS_2, &status2, 1);
  return status2;
}

uint8_t DepartmentOfAlchemy_FlashSPI::readStatus3() {
  uint8_t status3;
  readCommand(DOA_FLASH_CMD_READ_STATUS_3, &status3, 1);
  return status3;
}

void DepartmentOfAlchemy_FlashSPI::waitUntilReady() {
  // both "Erase/Write in Progress (BUSY)" and "Write Enable Latch (WEL)" should be clear
  //while (readStatus1() & (BIT_BUSY | BIT_WEL)) {
  while (readStatus1() & 0x03) {
    yield();
  }
}

bool DepartmentOfAlchemy_FlashSPI::writeEnable() {
  return runCommand(DOA_FLASH_CMD_WRITE_ENABLE);
}

bool DepartmentOfAlchemy_FlashSPI::writeDisable() {
  return runCommand(DOA_FLASH_CMD_WRITE_DISABLE);
}

uint32_t DepartmentOfAlchemy_FlashSPI::readBuffer(uint32_t address, uint8_t *buffer, uint32_t len) {
  waitUntilReady();

  bool const rc = readMemory(address, buffer, len);

  return rc ? len : 0;
}

uint32_t DepartmentOfAlchemy_FlashSPI::writeBuffer(uint32_t address, uint8_t const *buffer, uint32_t len) {
  // write, or program, one page (256 bytes) at a time.
  uint32_t remain = len; // keep track of how much still has to be "programmed"

  while (remain) {
    waitUntilReady(); // wait for any previous write to finish
    writeEnable(); // prepare to write

    // 256 - position in page
    uint32_t const leftOnPage = DOA_FLASH_PAGE_SIZE - (address & (DOA_FLASH_PAGE_SIZE - 1));
    // remaining amount or amount of bytes left on page
    uint32_t const toWrite = min(remain, leftOnPage);

    if (!writeMemory(address, buffer, toWrite)) {
      break;
    }

    remain -= toWrite; // subtract what was written from how much is left to write
    buffer += toWrite; // move buffer index forward how much was written
    address += toWrite; // move address index forward how much was written
  }

  len -= remain;

  return len;
}

bool DepartmentOfAlchemy_FlashSPI::eraseSector(uint32_t sectorNumber) {
  waitUntilReady(); // wait for any previous write to finish
  writeEnable(); // prepare to erase

  bool const ret = eraseCommand(DOA_FLASH_CMD_SECTOR_ERASE, sectorNumber * DOA_FLASH_SECTOR_SIZE);

  return ret;
}
