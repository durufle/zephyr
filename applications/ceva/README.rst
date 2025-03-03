.. _ceva_application:

.. raw:: html

Chip Evaluation firmware
########################

Overview
********
This is an application running on top of Zephyr OS. This application propose commands 
used for characterization purpose. 

Pre-requisite
*************
We assume that the zephyr environment is available.

Supported boards
****************

This application has been successfuly tested on the following targets:

#. `nrf5340-dk`_
#. `Arduino due`_
#. `nucleo l476rg`_
#. `nucleo g071rb`_
#. `nucleo l552ze_q`_
#. `nucleo l552ze_q`_
#. `nucleo h743zi`_
#. `nucleo f401re`_
#. `nucleo f446re`_


UART and Triggers
*****************
- By default the virtual communication uart is used by the application.

- AdditionnalyGPIOs are used by the firmware in order to trig external device, during
  command process. Refer to the board's overlay file to know which PIN number is used. 

Test script
***********
A python script is available to play with the firmware. See 

.. literalinclude:: ./scripts/test_command.py

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
.. _Arduino due: https://docs.zephyrproject.org/latest/boards/arduino/due/doc/index.html
.. _nrf5340 dk: https://docs.zephyrproject.org/latest/boards/nordic/nrf5340dk/doc/index.html
.. _nucleo l476rg: https://docs.zephyrproject.org/latest/boards/st/nucleo_l476rg/doc/index.html
.. _nucleo g071rb: https://docs.zephyrproject.org/latest/boards/st/nucleo_g071rb/doc/index.html
.. _nucleo l552ze_q: https://docs.zephyrproject.org/latest/boards/st/nucleo_l552ze_q/doc/nucleol552ze_q.html
.. _nucleo h743zi: https://docs.zephyrproject.org/latest/boards/st/nucleo_h743zi/doc/index.html 
.. _nucleo f401re: https://docs.zephyrproject.org/latest/boards/st/nucleo_f401re/doc/index.html
.. _nucleo f446re: https://docs.zephyrproject.org/latest/boards/st/nucleo_f446re/doc/index.html