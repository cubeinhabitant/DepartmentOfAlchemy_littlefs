/**
 * Deparment Of Alchemy
 * 
 * Example of using the lfsAdapter to use littlefs to access SPI flash.
 * Shows how an alternate SPI peripheral on a Samd21 can be used.
 * 
 */

#include <SPI.h>
#include "wiring_private.h" // Needed to create alternative SPI interface
#include "DepartmentOfAlchemy_lfsAdapter.h"

#define PIN_MISO 3
#define PIN_MOSI 4
#define PIN_SCK  5
SPIClass flashSPI(&sercom2, PIN_MISO, PIN_SCK, PIN_MOSI, SPI_PAD_0_SCK_3, SERCOM_RX_PAD_1);

#define SS 6

// Custom SPI
DepartmentOfAlchemy_FlashSPI flash(SS, flashSPI, true);
// Standard SPI
//DepartmentOfAlchemy_FlashSPI flash(SS, SPI);

DepartmentOfAlchemy_lfsAdapter lfsAdapter = DepartmentOfAlchemy_lfsAdapter(flash);
lfs_t lfs;
lfs_file_t file;


// from: https://github.com/littlefs-project/littlefs/issues/2

int lfs_ls(lfs_t *lfs, const char *path) {
    lfs_dir_t dir;
    int err = lfs_dir_open(lfs, &dir, path);
    if (err) {
        return err;
    }

    struct lfs_info info;
    while (true) {
        int res = lfs_dir_read(lfs, &dir, &info);
        if (res < 0) {
            return res;
        }

        if (res == 0) {
            break;
        }

        switch (info.type) {
            case LFS_TYPE_REG: Serial.print("reg "); break;
            case LFS_TYPE_DIR: Serial.print("dir "); break;
            default:           Serial.print("?   "); break;
        }

        static const char *prefixes[] = {"", "K", "M", "G"};
        for (int i = sizeof(prefixes)/sizeof(prefixes[0])-1; i >= 0; i--) {
            if (info.size >= (1 << 10*i)-1) {
                //printf("%*u%sB ", 4-(i != 0), info.size >> 10*i, prefixes[i]);
                for (int j = 0; j < 4-(i != 0); j++) {
                  Serial.print(" ");
                }
                Serial.print(info.size >> 10*i);
                Serial.print(prefixes[i]);
                Serial.print("B ");
                break;
            }
        }

        Serial.println(info.name);
    }

    err = lfs_dir_close(lfs, &dir);
    if (err) {
        return err;
    }

    return 0;
}

// ----------------

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // custom SPI
  flashSPI.begin();

  // Assign pins 3, 4, and 5 to SERCOM & SERCOM_ALT
  pinPeripheral(PIN_MISO, PIO_SERCOM_ALT);
  pinPeripheral(PIN_MOSI, PIO_SERCOM_ALT);
  pinPeripheral(PIN_SCK, PIO_SERCOM);

//  pinMode(SS, OUTPUT);
//  digitalWrite(SS, HIGH);

  Serial.println("Using non-standard SPI port");
//  Serial.println("Using standard SPI port");

/*
  // Working code which queries the flash memory via SPI
  uint8_t jedec_id[4];

  readCommand(CMD_JEDEC_ID, jedec_id, 4);
  Serial.print("JEDEC 0: ");
  Serial.println(jedec_id[0], HEX);
  Serial.print("JEDEC 1: ");
  Serial.println(jedec_id[1], HEX);
  Serial.print("JEDEC 2: ");
  Serial.println(jedec_id[2], HEX);
  Serial.print("JEDEC 3: ");
  Serial.println(jedec_id[3], HEX);

  uint8_t manufacturer_id[5];

  readCommand(CMD_MANUFACTURER, manufacturer_id, 5);
  Serial.print("MANU 0: ");
  Serial.println(manufacturer_id[0], HEX);
  Serial.print("MANU 1: ");
  Serial.println(manufacturer_id[1], HEX);
  Serial.print("MANU 2: ");
  Serial.println(manufacturer_id[2], HEX);
  Serial.print("MANU 3: ");
  Serial.println(manufacturer_id[3], HEX);
  Serial.print("MANU 4: ");
  Serial.println(manufacturer_id[4], HEX);

  uint8_t unique_id[6];

  readCommand(CMD_READ_UNIQUE_ID, unique_id, 6);
  Serial.print("UNIQ 0: ");
  Serial.println(unique_id[0], HEX);
  Serial.print("UNIQ 1: ");
  Serial.println(unique_id[1], HEX);
  Serial.print("UNIQ 2: ");
  Serial.println(unique_id[2], HEX);
  Serial.print("UNIQ 3: ");
  Serial.println(unique_id[3], HEX);
  Serial.print("UNIQ 4: ");
  Serial.println(unique_id[4], HEX);
  Serial.print("UNIQ 5: ");
  Serial.println(unique_id[5], HEX);
*/

// Don't need if DeparmentOfAlchemy_lfsAdapter is being used for begin
/*
    if (!flash.begin()) {
      Serial.println(F("Failed to initialize flash"));
      while(1); // don't do anything more
    }

    Serial.println(F("Flash initialized"));
*/

    // clear any memory protection

//    flash.writeEnable();
//    uint8_t s[1] = {0x00};
//    flash.writeCommand(DOA_FLASH_CMD_WRITE_STATUS_1, s, 1);
//    delay(1);

/*
    //
    // Testing of the DeparmentOfAlchemy_FlashSPI
    // - eraseSector
    // - writeBuffer
    // - readBuffer
    //
    uint8_t buf[3];
    uint32_t len;
*/

/*
    Serial.println(F("Erasing sector."));
    if (!flash.eraseSector(1)) {
      Serial.println(F("Failed to erase flash sector"));
      while(1); // don't do anything more
    }


    // check to see if erased
    Serial.println(F("Check to see if erased..."));
    len = flash.readBuffer(4096, buf, 3);
    if (len != 3) {
      Serial.println(F("Failed to read buffer"));
      while(1); // don't do anything more
    }
    
    if (buf[0] != 0xFF) {
      Serial.print(F("Unexpected byte 0: "));
      Serial.println(buf[0], HEX);
      while(1); // don't do anything more
    }

    if (buf[1] != 0xFF) {
      Serial.print(F("Unexpected byte 1: "));
      Serial.println(buf[1], HEX);
      while(1); // don't do anything more
    }

    if (buf[2] != 0xFF) {
      Serial.print(F("Unexpected byte 2: "));
      Serial.println(buf[2], HEX);
      while(1); // don't do anything more
    }
    Serial.println(F("Erased."));
*/

//    uint8_t b[3] = {0x05, 0x08, 0x0F};

/*
    Serial.println(F("Writing sample data"));
    len = flash.writeBuffer(254, b, 3);
    if (len != 3) {
      Serial.println(F("Failed to write buffer"));
      while(1); // don't do anything more
    }
    Serial.println(F("Data written"));
*/
/*
    len = flash.readBuffer(254, buf, 3);
    if (len != 3) {
      Serial.println(F("Failed to read buffer"));
      while(1); // don't do anything more
    }

    if (buf[0] != 0x05) {
      Serial.print(F("Unexpected byte 0: "));
      Serial.println(buf[0], HEX);
      while(1); // don't do anything more
    }

    if (buf[1] != 0x08) {
      Serial.print(F("Unexpected byte 1: "));
      Serial.println(buf[1], HEX);
      while(1); // don't do anything more
    }

    if (buf[2] != 0x0F) {
      Serial.print(F("Unexpected byte 2: "));
      Serial.println(buf[2], HEX);
      while(1); // don't do anything more
    }

    Serial.println(F("Write confirmed"));
    //
    // END - Testing of the DeparmentOfAlchemy_FlashSPI
*/

  //
  // Test DeparmentOfAlchemy_lfsAdapter
  //

  if (!lfsAdapter.begin()) {
    Serial.println(F("Failed to initialize lfsAdapter"));
    while(1); // don't do anything more
  }

  Serial.println(F("lfsAdapter initialized"));

  struct lfs_config cfg = lfsAdapter.getConfig();

  int err = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err) {
    err = lfs_format(&lfs, &cfg);
    if (err) { Serial.print("lfs_format error: "); Serial.println(err); }
    err = lfs_mount(&lfs, &cfg);
    if (err) { Serial.print("lfs_mount error: "); Serial.println(err); }
  }

  // read current count
  uint32_t boot_count = 0;
  lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
  lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

  // update boot count
  boot_count += 1;
  lfs_file_rewind(&lfs, &file);
  lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

  // remeber the storage is not updated until the file is closed successfully
  lfs_file_close(&lfs, &file);

  // print the boot count
  Serial.print("boot_count: ");
  Serial.println(boot_count);

  // list directory
  lfs_ls(&lfs, "/");

  // release any resources we were using
  lfs_unmount(&lfs);
}

void loop() {
  // put your main code here, to run repeatedly:

}
