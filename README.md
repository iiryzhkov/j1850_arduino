# J1850 lib

**The library for arduino is working with the protocol j1850. It allows you to receive and send messages, work with it and output it to the console.**

## API

### Include

`#include <j1850.h>`

Create an instance of the class
for example, create the object 'j'.

`j1850 j;`

### Init

`void j1850::init(int RX_PIN, int TX_PIN, Print* pr)`

- RX_PIN - port for incoming signal
- TX_PIN - port for outgoing signal
- pr -     serial port output class (for example: Serial - default or SerialUSB) 

for example:

`j.init(10, 11, &SerialUSB);`


#### Debugging mode

`j.set_monitoring(int mode)`

- mode - data output mode to the serial port 

It allows you to output incoming messages, outgoing messages, status codes and run tests. 
- 0 - turn off monitor;
- 1 - incoming and outgoing messages;
- 2 - status codes;
- 3 - incoming messages;
- 4 - outgoing messages;
- 5 - tests.

### Read the messages

`bool j1850::accept(byte *msg_buf, bool crt_c)`

- msg_buf - buffer for received messages
- crt_c - check hash (default false)

returns the receive status:
- 1 - successfully
- 0 - failure

for example:
Before receiving messages, you must create a receive buffer. Protocol j1850 allows you to send messages up to **12 characters**.

`byte rx_buf[12];`

Receiving a message

`j.accept(rx_buf)`

the class property **'rx_nbyte'** will contain the number of characters.

### Sending a message

There are two methods to send a message:

`bool j1850::send(byte *tx_buf, int nbytes)`

- tx_buf - send buffer
- nbytes - number of characters sent (0 - 11)

returns the sending status:
- 1 - successfully
- 0 - failure

for example:

`byte tx_buf[] = {0x00, 0x01, 0x03}`
`j.send(tx_buf, 3)`

and

`bool j1850::easy_send(int size, ...)`

- size - number of characters sent (0 - 11)
- sent characters

returns the sending status:
- 1 - successfully
- 0 - failure

for example:

`j.easy_send(3, 0x00, 0x01, 0x03)`

In the end of the message the crc sum will be automatically added.
The number of sent characters will be written in **'tx_nbyte'**

### Status codes
The **'message'*** property contains event codes:

- 1 - MESSAGE_SEND_OK
- 2 - MESSAGE_ACCEPT_OK
- 3 - ERROR_MESSAGE_TO_LONG
- 4 - ERROR_NO_RESPONDS_WITHIN_100US
- 5 - ERROR_ON_SOF_TIMEOUT
- 6 - ERROR_SYMBOL_WAS_NOT_SOF
- 7 - ERROR_SYMBOL_WAS_NOT_SHORT
- 8 - ERROR_ACCEPT_CRC
- 9 - ERROR_SEND_COLLISION

### Params

- ATTEMPT_TO_SEND -  The number of attempts to send a packet
- TOLERANCE - Permissible deviation from the duration of the pulse. Let us say, transmit signal 100 ms, means 100 +(-) 100\2 = 150(50)ms


# API distributed work

Allows you to work with j1850 on another controller and link it with the main controller on I2C. Devices must be connected by all rules. See the instructions for the library Wire.

## slave

### Include

`#include <j1850_distributed.h>`

Create an instance of the class
for example, create the object 'j'.

`j1850_slave j;`

### Init

`void j1850_slave::init(int RX_PIN, int TX_PIN, Print* pr, int address, long speed)`

- RX_PIN - port for incoming signal
- TX_PIN - port for outgoing signal
- pr -     serial port output class (for example: Serial - default or SerialUSB)
- address - I2C address of this device. Default value ADDRESS_I2C
- speed - I2C speed. Default value SPEED_I2C

for example:

`j.init(10, 11, &SerialUSB, 3400000, 8);`

#### Debugging mode

`j.set_monitoring(int mode)`

- mode - data output mode to the serial port 

It allows you to output incoming messages, outgoing messages, status codes and run tests. 
- 0 - 5 - just like j1850
- 6 - status of I/O buffers

#### Filter

`void set_filter(func)`

- func - linc on function

Sets the filter function.

`bool filter(byte *data, int len)`

- data - link to the array with the message
- len - len to this array

for example:
`bool filter(byte *data, int len){`
    `return true;`
`}`
`j.set_filter(&filter);`

`j.set_monitoring(int mode)`

### Work

`void loop();`

It is necessary to call each cycle.

for example:
`j.loop();`

#### Buffers

`bool len_buffer(bool _print=false);`

Check buffer btatus

- _print - state output to console
- return - data availability in buffers

### Status codes
The **'message'*** property contains event codes:

- 1 - 9 - just like j1850
- 10 - J1850_MESSAGE_FILTERED
- 11 - WIRE_WRITE_BUFFER_IS_NOT_EMPTY
- 12 - WIRE_READ_BUFFER_IS_NOT_EMPTY
- 13 - WIRE_WRITE_OK
- 14 - WIRE_READ_OK

### Params

- SPEED_I2C - I2C speed
- ADDRESS_I2C - I2C address of this device
- LEN_BUFFER_R - Read buffer size
- LEN_BUFFER_W - Write buffer size

## Master

### Include

`#include <j1850_distributed.h>`

Create an instance of the class
for example, create the object 'j'.

`j1850_master j;`

### Init

`void j1850_master::init(int address, long speed, Print* pr,)`

- pr -     serial port output class (for example: Serial - default or SerialUSB)
- address - I2C slave address. Default value ADDRESS_I2C
- speed - I2C speed. Default value SPEED_I2C

for example:

`j.init(3400000, 8, &SerialUSB);`

### Read the messages

`bool j1850_master::accept(byte *msg_buf, bool timeout)`

- msg_buf - buffer for received messages
- timeout - limit call frequency to TIMEOUT_ACCEPT_DATA_US

returns the receive status:
- 1 - successfully
- 0 - failure

for example:
Before receiving messages, you must create a receive buffer. Protocol j1850 allows you to send messages up to **12 characters**.

`byte rx_buf[12];`

Receiving a message

`j.accept(rx_buf)`

### Sending a message

There are two methods to send a message:

`bool j1850::send(byte *tx_buf, int nbytes)`

and

`bool j1850::easy_send(int size, ...)`

Work as in J1850

### Status codes
The **'message'*** property contains event codes:

- 1 - MESSAGE_SEND_OK
- 2 - MESSAGE_ACCEPT_OK

### Params

- TIMEOUT_ACCEPT_DATA_US - data retrieval timeout
