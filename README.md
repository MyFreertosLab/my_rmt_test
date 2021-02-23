ESP-IDF RMT Test
================

This is a test of a six channel RC Receiver connected on a single GPIO Pin via a eight channels PPM Encoder.

The software configures the RMT module and starts it.
It receives the values of eight PPM channels transmitted by the PPM Encoder in a circular buffer and displays them.


Below are the components used:

<h1>Hw Components</h1>
<p align="left">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/esp32.jpg" width="300" title="The Esp32 Board">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/receiver.jpg" width="300" title="The connections between the Board, the Receiver and the PPM Encoder">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/transmitter.jpg" width="300" title="The RC Transmitter">
  <img src="https://github.com/MyFreertosLab/my_rmt_test/blob/master/images/logs.jpg" width="300" title="The logs">
</p>

