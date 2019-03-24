#include <j1850_distributed.h>

//Create an instance of the class
j1850_master j;

void setup() {
    Serial.begin(2000000);
    // initialization with default values
    j.init();
    j.set_monitoring(1);
}

void loop() {
  //Buffer for recording received messages
  byte rx_buf[12];
  
  //Receiving a message
  if (j.accept(rx_buf, true)) {
    //Sending the received message
    j.send(rx_buf, j.rx_nbyte);

    //Sending a static message FF, FF, FF, FF, FF, CRC sum
    j.easy_send(5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF);
  }
}
