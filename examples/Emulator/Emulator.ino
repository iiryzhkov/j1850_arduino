#include <j1850.h>

//Create an instance of the class
j1850 j;

void setup() {
    Serial.begin(115200);
    //Initialize. RX pin, TX pin
    j.init(3, 4, &Serial);
    j.set_monitoring(0);
}

void loop() {
  //Buffer for recording received messages
  byte rx_buf[12];
  if (Serial.available() > 0) j.set_monitoring(Serial.parseInt());
  //Receiving a message
  if (!j.accept(rx_buf, true) or (rx_buf[0] != 0x8D) or (rx_buf[1] != 0x0F)){
    return;
  }
  switch(rx_buf[2]){
    case 0x21:
      j.easy_send(5, 0x8D, 0x92, 0xC0, 0x00, 0x00);
      j.easy_send(5, 0x8D, 0x92, 0xE1, 0x01, 0x03);
      j.easy_send(5, 0x8D, 0x92, 0xE3, 0x01, 0x99);
      break;
    case 0x24:
      j.easy_send(4, 0x8D, 0x94, 0x00, 0x00);
      break;
    default:
      break;
  }
  j.easy_send(5, 0x8D, 0x93, 0x01, 0x01, 0x80);
}