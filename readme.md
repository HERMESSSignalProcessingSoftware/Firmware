# MicroController

Smartfusion 2 is a system on chip (SoC) incorporating an fpga fabric and a microcontroller subsystem (MSS).

## Setting up flashing environment for updating only
1. Download the newest version of [Flashpro Express](https://www.microchip.com/en-us/products/fpgas-and-plds/fpga-and-soc-design-tools/programming-and-debug/flashpro-and-flashpro-express#overview). If you have Libero SoC installed, you have your Flashpro Express executable located at `LiberoSoCInstallationPath/Designer/bin/FPExpress.exe`.
2. Download the newest Flashpro express job file [release](https://github.com/HERMESSSignalProcessingSoftware/Firmware/releases).
3. Open Flashpro Express and create a new project using the job file downloaded in step 2.
4. Run the PROGRAM action

## Setting up working environment for development
Smartfusion 2 design. Use [Libero SOC v12](https://www.microsemi.com/product-directory/design-resources/1750-libero-soc)
with silver license.

1. Clone master branch (ignore the rest)
2. Install Libero SoC 2021.3 (no other version!)
3. Install SoftConsole 2021.3
4. Open Libero project. Initial errors may occur due to invalid paths. Those will be gone by the next time you start up the application
5. In eNVM configuration remove the option to program the `program` and `conf` client.