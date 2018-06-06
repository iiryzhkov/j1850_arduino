 #include <j1850.h>

//Create an instance of the class
j1850 j;

void setup() {
    SerialUSB.begin(9600);
    //Initialize. RX pin ,TX pin Asynch mod
    j.init(3, 5, true, &SerialUSB);
    j.set_monitoring(1);
}

void loop() {
  //Receiving a message
  if (j.bytes > 0) {
    //Sending the received message
    j.send(j.accept_buf, j.bytes - 1);
    j.reset_accept();
  }
} 
