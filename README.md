ESP-IDF RMT Test
================

This is a test of a six channel RC Receiver connected on a single GPIO Pin via a eight channels PPM Encoder.

The software configures the RMT module and starts it.
It receives the values of eight PPM channels transmitted by the PPM Encoder in a circular buffer and displays them.

You can build it with command <code>cmake --build build</code> or  <code>idf.py build</code>

To try it out, you should:
1) connect the RC Receiver power to the Board
2) connect PPM Encoder to the Board (I used GPIO23)
3) connect your development board on USB port
4) flash the firmware with the command <code>idf.py flash</code>
5) run <code>idf.py monitor</code> to see the logs
6) Use the transmitter to check the values

Below are the components used:

<h1>Hw Components</h1>
<p align="left">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/esp32.jpg" width="300" title="The Esp32 Board">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/receiver.jpg" width="300" title="The connections between the Board, the Receiver and the PPM Encoder">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/logs.jpg" width="300" title="The logs">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/transmitter.jpg" width="300" title="The RC Transmitter">
</p>

