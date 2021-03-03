# Department of Alchemy littlefs SPI Flash library

This is an SPI Flash implementation of the littlefs filesystem. It has been test with
a SAMD21 processor in the Arduino Zero. It currently only supports the Winbond
25Q64JVSIQ 64 megabit (8 megabyte) flash chip.

littlefs is a "little fail-safe filesystem designed for microcontrollers.": https://github.com/littlefs-project/littlefs

## Getting Started

The library is very much in beta right now. It has successfully been run with an Arduino Zero
and a Winbond 25Q64JVSIQ 64 megabit (8 megabyte) flash chip.

### Installing

This driver can be installed in the Arduino IDE by downloading the ZIP file. In the Arduino IDE, choose "Sketch" > "Include Library" > "Add .ZIP Library..." and choose the ZIP file downloaded.

### Usage

Check the examples in the library for examples showing how to use the library.

* DeparmentOfAlchemy_littlefs - Create a littlefs filesystem with an alternate SPI peripheral
                                available on the SAMD12 M0+ chip.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags).

## Alternative Flash RAM libraries
* [Adafruit_SPIFlash](https://github.com/adafruit/Adafruit_SPIFlash) - Much of the SPI flash chip
interface was inspired by this implementation.
* [SPIMemory](https://github.com/Marzogh/SPIMemory) - General purpose SPI flash memory library.

## Authors

* **Jesse Peterson** - *Initial work* - [cubeinhabitant](https://github.com/cubeinhabitant)

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details