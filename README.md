# JTAG-Permutator
A low-cost USB tool for identifying pinouts of possible JTAG programming/debug interfaces on electronics.
<p align="center">
  <img src="https://i.imgur.com/cmikTXL.jpg" width="250"/>
  <img src="https://imgur.com/1QK74E8.jpg" width="250"/>
  <img src="https://imgur.com/mtfjIon.jpg" width="250"/>
</p>

## License
The JTAG Permutator is an adaptation of the "[JTAGulator](https://github.com/grandideastudio/jtagulator)" designed by Joe Grand of [Grand Idea Studio](http://www.grandideastudio.com/) and is used under the [Creative Commons Attribution 3.0 United States license](https://creativecommons.org/licenses/by/3.0/us/).

The JTAG Permutator design is also distributed under the [Creative Commons Attribution 3.0 United States license](https://creativecommons.org/licenses/by/3.0/us/).

### Major changes from the JTAGulator design
1. Silicon Labs EFM8 microcontroller used instead of Parallax microcontroller with USB-UART bridge IC
2. Target voltage supplied by SC195 buck regulator rather than filtered PWM
3. Removed screw terminals
4. Reduced IO channels from 24 down to 8
5. Used different level translator IC (target voltage range is now 0.8V~3.3V) (to be verified)
6. Reduced board area from 13.6 square inches down to 1.7 square inches
