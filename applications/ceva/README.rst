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
The supported boards are:

#. `Nordic nRF5340 DK`_
#. `Arduino DUE`_
#. `Infineon CY8CKIT-062-BLE`_
#. `nucleo_f401re`_
#. `nucleo_g071rb`_
#. `nucleo_h743zi`_
#. `nucleo_l552ze_q`_
#. `nucleo_u575zi_q`_


UART and Triggers
*****************
- By default the virtual communication uart is used by the application.

- Additionnaly GPIOs are used by the firmware in order to trig external device, during
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
- Push erase button for a few seconds, then push reset button for a short period, wait a second then
- execute the following command again:

.. code-block::

  west flash

  
 Reference
 *********

.. _Nordic nRF5340 DK: https://www.nordicsemi.com/Products/Development-hardware/nRF5340-DK
.. _Arduino DUE: https://store.arduino.cc/products/arduino-due
.. _Infineon CY8CKIT-062-BLE: https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062-ble/#
.. _nucleo_f401re: https://www.st.com/en/evaluation-tools/nucleo-f401re.html
.. _nucleo_g071rb: https://www.st.com/en/evaluation-tools/nucleo-g071rb.html
.. _nucleo_h743zi: https://www.st.com/en/evaluation-tools/nucleo-h743zi.html
.. _nucleo_l476rg: https://www.st.com/en/evaluation-tools/nucleo-l476rg.html
.. _nucleo_l552ze_q: https://www.st.com/en/evaluation-tools/nucleo-l552ze-q.html
.. _nucleo_u575zi_q: https://www.st.com/en/evaluation-tools/nucleo-u575zi-q.html



