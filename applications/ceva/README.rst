.. _ceva_application:

Chip evaluation firmware
########################

Overview
********
This is an application running on top of Zephyr OS.

Pre-requisite
*************
We assume that the zephyr environment is available.

Supported boards
****************

This application has been successfuly tested on the folwing targets:

* - Targets
  - link
* - Arduino due
  - nrf5340dk
  - nucleo-l476rg
  - nucleo-l552ze_q
  - nucleo-h743zi
  - nucleo_g071rb
* - Arduino due <https://docs.zephyrproject.org/latest/boards/arduino/due/doc/index.html>_
  - nrf5340 DK <https://docs.zephyrproject.org/latest/boards/nordic/nrf5340dk/doc/index.html>_
  - nucleo-l476rg <https://docs.zephyrproject.org/latest/boards/st/nucleo_l476rg/doc/index.html>_
  -
  -
  -


UART and Triggers
*****************
- By default the virtual communication uart is used by the application.

- Additionnaly GPIOs are used by the firmware in order to trig external device, during
  command process. Refer to the board's overlay file to know which PIN number is used. 

Test script
***********
A python script is available to play with the firmware. See 

.. literalinclude:: ./script/test_command.py

Building and Running
********************
Build and flash the sample as follows, changing ``nrf52840dk_nrf52840_cpuapp`` for
your board:

.. code-block::
   west build -p always -b nrf52840dk_nrf52840_cpuapp application/ceva
   west flash

For arduino due if you have trouble to flash, do the following:

- If port access denied, then refer to `Arduino Fix Port Access`_
- Push erase button for a few seconds, then push reset button for a short period, wait a second then
- execute the following command again:

.. code-block::
  west flash
 

.. _Arduino Fix Port Access: https://support.arduino.cc/hc/en-us/articles/360016495679-Fix-port-access-on-Linux
