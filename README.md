# J1850 lib

**The library for arduino is working with the protocol j1850. It allows you to receive and send messages, work with it and output it to the console.**

## API

### Include

`#include <j1850.h>`

Create an instance of the class
for example, create the object 'j'.

`j1850 j;`

### Init

`void j1850::init(int RX_PIN, int TX_PIN, bool debug)`

- RX_PIN - port for incoming signal
- TX_PIN - port for outgoing signal
- debug - enable / disable debugging mode (default false)

for example:

`j.init(10, 11, true);`


#### Debugging mode
It allows you to output incoming messages, outgoing messages, status codes and run tests. 
To change the modes, you need to send to the console:
- 1 (default) -  incoming and outgoing messages;
- 2 - status codes;
- 3 - incoming messages;
- 4 - outgoing messages;
- 5 - tests;

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
- 8 - define ERROR_ACCEPT_CRC