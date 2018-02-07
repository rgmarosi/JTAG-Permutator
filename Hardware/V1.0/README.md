# Notes
- IDC headers are not placed in BOM because fab & assembly is cheaper with all SMD components. They can be easily hand-soldered.
- The JTAG Permutator is compatible with [Bus Pirate V3 probes](https://www.seeedstudio.com/Bus-Pirate-v3-probe-Kit-p-526.html) or equivalent.
- The programmer interface requires a stronger 1kOhm pull-up resistor on the reset pin in order for the debug adapter to remain connected to the JTAG-Permutator
  - Solder a 1kOhm resistor between TP1 and TP5 on the rear of the board to fix this
- The programmer used with the programmer header is the Silicon Labs [8-bit USB Debug Adapter](https://www.silabs.com/products/development-tools/mcu/8-bit/8bit-mcu-accessories/8-bit-debug-adapter)
  - The programmer header may not be needed if I can figure out how to use the bootloader on the EFM8. Until then, it's there for easy debugging of this first prototype.
- The fab & assembly house I used for this first prototype was [MacroFab](https://macrofab.com/). Another good self-serve fab & assembly house is [PCB:NG](https://www.pcb.ng/).
- I don't yet know if the onboard buck regulator (SC195) output is tri-stated when disabled. I need to test it. For now, don't connect the target power rail to powered targets. Only use it if you're powering the target using the JTAG-Permutator.
  - Maximum current output of the buck regulator is ~500mA.

# Planned changes
1. Pull up C2CK pin of microcontroller to 5V rail using 1kOhm resistor.
2. Remove programming header once bootloader is figured out and firmware is functioning as intended. Keep C2 interface and power rail test points.
3. More to come...
