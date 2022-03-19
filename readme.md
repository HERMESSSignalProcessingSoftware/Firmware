# MicroController

Smartfusion 2 design. Use [Libero SOC v12](https://www.microsemi.com/product-directory/design-resources/1750-libero-soc)
with silver license.

Smartfusion 2 is a system on chip (SoC) incorporating an fpga fabric and a microcontroller subsystem (MSS).

## Setting up working environment

1. Clone master branch (ignore the rest)
2. Install Libero SoC 2021.3 (no other version!)
3. Install SoftConsole
4. Open Libero project. Initial errors may occur due to invalid paths. Those will be gone by the next time you start up the application
5. In eNVM configuration remove the option to program the `program` and `conf` client.