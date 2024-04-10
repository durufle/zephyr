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
Refer to the application's boards directory.

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
Build and flash the sample as follows, changing ``nrf52840dk_nrf52840`` for
your board:

.. code-block::
   west build -p always -b nrf52840dk_nrf52840 application/ceva
   west flash

For arduino due if you have trouble to flash, do the following:
- Push erase button for a few seconds, then push reset button for a short period, wait a second then
- execute the following command again:

.. code-block::
  west flash

  
 

