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
  byte tmp_buf[12];
  int tmp_len;
  //Receiving a message
  if (j.bytes > 0) {
    for(int i = 0; i < j.bytes - 1; i++){
      tmp_buf[i] = j.accept_buf[i];
    }
    tmp_len = j.bytes - 1;
    //Sending the received message
    j.send(tmp_buf, tmp_len);
    j.reset_accept();
  }
  if (j.message != 0){
    		SerialUSB.println(j.message);
        j.message = 0;
  }
} 
