#include <j1850.h>

//Create an instance of the class
j1850 j;

void setup() {
    SerialUSB.begin(2000000);
    //Initialize. RX pin, TX pin
    j.init(3, 5, false, &SerialUSB);
    j.set_monitoring(1);
}

void loop() {
  //Buffer for recording received messages
  byte rx_buf[12];
  
  //Receiving a message
  if (j.accept(rx_buf)) {
    //Sending the received message
    j.send(rx_buf, j.rx_nbyte - 1);

    //Sending a static message FF, FF, FF, FF, FF, CRC sum
    j.easy_send(5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  }
}
