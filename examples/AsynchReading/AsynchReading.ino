 #include <j1850_asynch_exc.h>

//Create an instance of the class
j1850_asynch_exc j;

void sendToUART(const char *header, volatile int nbyte, volatile byte *msg_buf) {
	SerialUSB.print(header);
	for (int i = 0; i < nbyte; i++) {
		if (msg_buf[i] < 0x10)
			SerialUSB.print(0);

		SerialUSB.print(msg_buf[i], HEX);
		SerialUSB.print(" ");
	}
	SerialUSB.print("\n");
}

void setup() {
		// Init tx
		pinMode(5, OUTPUT);
	  digitalWrite(5, HIGH);
    SerialUSB.begin(2000000);
    //Initialize. RX pin
    j.asynch_init(3);
}

void loop() {
  if (j.bytes > 0) {
    sendToUART("RX: ", j.bytes, j.accept_buf);
    j.reset_accept();
  }
} 
