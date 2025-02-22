
Overview
********
This is an application running on top of Zephyr OS.

Pre-requisite
*************
We assume that the zephyr environment is available.

Supported boards
****************

This application has been successfuly tested on the folwing targets:

.. list-table:: Supported targets
   :widths: 25 25
   :header-rows: 1

  * - Targets
    - Informations
  * - Arduino due
    - `Arduino due board <https://docs.zephyrproject.org/latest/boards/arduino/due/doc/index.html>`_
  * - nrf5340dk
    - `nrf5340-DK board <https://docs.zephyrproject.org/latest/boards/nordic/nrf5340dk/doc/index.html>`_
  * - nucleo-l476rg
    - `nucleo-l476rg board <https://docs.zephyrproject.org/latest/boards/st/nucleo_l476rg/doc/index.html>`_
  * - nucleo-g071rb
    - `nucleo-g071rb board <https://docs.zephyrproject.org/latest/boards/st/nucleo_g071rb/doc/index.html>`_
  * - nucleo-l552ze_q
    - `nucleo-l552ze_q board <https://docs.zephyrproject.org/latest/boards/st/nucleo_l552ze_q/doc/nucleol552ze_q.html>`_
  * - nucleo-h743zi
    - `nucleo-h743zi board <https://docs.zephyrproject.org/latest/boards/st/nucleo_h743zi/doc/index.html>`_
  * - nucleo-f401re
    - `nucleo-f401re board <https://docs.zephyrproject.org/latest/boards/st/nucleo_f401re/doc/index.html>`_
  * - nucleo-f446re
    - `nucleo-f446re board <https://docs.zephyrproject.org/latest/boards/st/nucleo_f446re/doc/index.html>`_

UART and Triggers
*****************

- By default the virtual communication uart is used by the application.

- Additionnaly GPIOs are used by the firmware in order to trig external device, during
  command process. Refer to the board's overlay file to know which PIN number is used. 

Test script
***********

A python script is available to play with the firmware. See 

.. literalinclude:: ../../scripts/test_command.py

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
