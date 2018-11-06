#include "j1850_distributed.h"
#include <Wire.h>

j1850_slave * j1850_slave::instances = NULL;

void j1850_slave::init(int in_pin_, int out_pin_, Print* pr_, int chenal, int speed) {
	TestSlave::init(in_pin_, out_pin_, pr_);
	Wire.begin(chenal);
	Wire.setClock(speed);
	Wire.onRequest(requestEvent);
	Wire.onReceive(receiveEvent);
	instances = this;
}

void j1850_slave::__receiveEvent(int howMany) {
  int i = 0;
  if (write_bytes > 0) {
    while (0 < Wire.available()) {
      Wire.read();
    }
  } else {
    while (0 < Wire.available()) {
      buff_write[i++] = Wire.read();
    }
    write_bytes = i;
  }
}

void j1850_slave::__requestEvent() {
  if (read_bytes > 0) {
    Wire.write(buff_read, read_bytes);
    read_bytes = 0;
  } else {
    Wire.write(0);
  }
}

void j1850_slave::loop() {
	byte tmp_read_buff[12];
	if (accept(tmp_read_buff, true)) {
		if (!filter(tmp_read_buff, rx_nbyte)) {
			return;
		}
		if (read_bytes == 0) {
			buff_read[0] = rx_nbyte;
			for (int i=0; i<rx_nbyte; i++) {
			buff_read[i + 1] = tmp_read_buff[i];
			}
			read_bytes = rx_nbyte + 1;
		}
	}
	if (write_bytes > 0) {
		send(buff_write, write_bytes);
		write_bytes = 0;
	}
}

 bool j1850_slave::filter(byte *data, int len) {
//    if ((len < 4) || (len > 6)) return false;
//    if ((data[0] != 0x8D) && (data[0] != 0x80) && (data[0] != 0x3D)) return false;
//    if ((data[0] != 0x80) && ((data[1] != 0x12) || (data[1] != 0x0F))) return false;
   return true;
 }

 void j1850_master::init(int _chenal, int speed, Print* pr_){
    chenal = _chenal;
    pr = pr_;
    Wire.begin();
    Wire.setClock(speed);
}

bool j1850_master::accept(byte *msg_buf, bool limit) {
    Wire.requestFrom(chenal, 13);
    int len =  Wire.read();
    for (int i = 0; i < len; i++) {
        msg_buf[i] = Wire.read();
    }
    rx_nbyte = len;

	if (monitoring_mode > 0)
		monitor();

	return (bool) len;
}

bool j1850_master::send(byte *msg_buf, int nbytes) {
    Wire.beginTransmission(chenal);
    Wire.write(msg_buf, nbytes);
    Wire.endTransmission();
    tx_nbyte = nbytes;
	if (monitoring_mode > 0){
		monitor();
	}
	return (bool)nbytes;
}