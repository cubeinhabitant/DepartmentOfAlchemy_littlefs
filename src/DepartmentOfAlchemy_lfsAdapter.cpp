
#include "DepartmentOfAlchemy_lfsAdapter.h"

// littlefs block device operations //
static int bd_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                                         void *buffer, lfs_size_t size) {
  DepartmentOfAlchemy_lfsAdapter *lfsAdapter = (DepartmentOfAlchemy_lfsAdapter *)c->context;
//  Serial.println("lfsAdapter bd_read called");
//  Serial.print("block: ");
//  Serial.print(block);
//  Serial.print(", off: ");
//  Serial.print(off);
//  Serial.print(", size: ");
//  Serial.println(size);
  return lfsAdapter->read(block, off, buffer, size);
}

static int bd_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off,
                                         const void *buffer, lfs_size_t size) {
  DepartmentOfAlchemy_lfsAdapter *lfsAdapter = (DepartmentOfAlchemy_lfsAdapter *)c->context;
//  Serial.println("lfsAdapter bd_prog called");
//  Serial.print("block: ");
//  Serial.print(block);
//  Serial.print(", off: ");
//  Serial.print(off);
//  Serial.print(", size: ");
//  Serial.println(size);
  return lfsAdapter->program(block, off, buffer, size);
}

static int bd_erase(const struct lfs_config *c, lfs_block_t block) {
  DepartmentOfAlchemy_lfsAdapter *lfsAdapter = (DepartmentOfAlchemy_lfsAdapter *)c->context;
//  Serial.println("lfsAdapter bd_erase called");
//  Serial.print("block: ");
//  Serial.println(block);
  return lfsAdapter->erase(block);
}

static int bd_sync(const struct lfs_config *c) {
  DepartmentOfAlchemy_lfsAdapter *lfsAdapter = (DepartmentOfAlchemy_lfsAdapter *)c->context;
//  Serial.println("lfsAdapter bd_sync called");
  return LFS_ERR_OK;
}

DepartmentOfAlchemy_lfsAdapter::DepartmentOfAlchemy_lfsAdapter(DepartmentOfAlchemy_FlashSPI *flashSPI) {
  _flashSPI = flashSPI;
  
  memset(&_config, 0, sizeof(_config));
  // TODO: these are provided in the constructor in the mbed implementation
  _config.block_size = 4096;
  _config.block_cycles = 500;
  _config.cache_size = 16;
  _config.lookahead_size = 16;

}

DepartmentOfAlchemy_lfsAdapter::DepartmentOfAlchemy_lfsAdapter(DepartmentOfAlchemy_FlashSPI &flashSPI)
  : DepartmentOfAlchemy_lfsAdapter(&flashSPI) {}

bool DepartmentOfAlchemy_lfsAdapter::begin() {
  return _flashSPI->begin();
}

struct lfs_config DepartmentOfAlchemy_lfsAdapter::getConfig() {
  // TODO: actually implement the configuration values
  // TODO: this is done in mbed in the LittleFileSystem2::mount method
  _config.context = this;

  // block device operations
  _config.read = bd_read;
  _config.prog = bd_prog;
  _config.erase = bd_erase;
  _config.sync = bd_sync;

  // block device configuration
  _config.read_size = 16;
  _config.prog_size = 16;
  _config.block_size = 4096; // sector erase size of 4K-bytes
  _config.block_count = _flashSPI->size() / _config.block_size;
  _config.block_cycles = 500;
  _config.cache_size = 16;
  _config.lookahead_size = 16;

  return _config;
}

int DepartmentOfAlchemy_lfsAdapter::read(lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size) {
//  Serial.println("lfsAdapter read called");
//  Serial.print("block: ");
//  Serial.print(block);
//  Serial.print(", off: ");
//  Serial.print(off);
//  Serial.print(", size: ");
//  Serial.println(size);
  int count = _flashSPI->readBuffer(block * _config.block_size + off, (uint8_t *)buffer, size);
  return count == size ? LFS_ERR_OK : LFS_ERR_IO;
}

int DepartmentOfAlchemy_lfsAdapter::program(lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size) {
//  Serial.println("lfsAdapter program called");
//  Serial.print("block: ");
//  Serial.print(block);
//  Serial.print(", off: ");
//  Serial.print(off);
//  Serial.print(", size: ");
//  Serial.println(size);
  int count = _flashSPI->writeBuffer(block * _config.block_size + off, (uint8_t *)buffer, size);
  return count == size ? LFS_ERR_OK : LFS_ERR_IO;
}

int DepartmentOfAlchemy_lfsAdapter::erase(lfs_block_t block) {
//  Serial.println("lfsAdapter erase called");
//  Serial.print("block: ");
//  Serial.println(block);
  if (!_flashSPI->eraseSector(block)) {
    return LFS_ERR_IO;
  }
  
  return LFS_ERR_OK;
}
