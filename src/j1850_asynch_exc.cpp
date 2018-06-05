#include "j1850_asynch_exc.h"

j1850_asynch_exc * j1850_asynch_exc::instances = NULL;

void j1850_asynch_exc::interrupt_on(void){
  interrupt_locking = false;
}

void j1850_asynch_exc::interrupt_off(void){
  interrupt_locking = true;
}

void j1850_asynch_exc::asynch_init(uint8_t interrupt) {
  in_pin = interrupt;
  pinMode(in_pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(in_pin), ISP, CHANGE);
  interrupt_on();
  instances = this;
}

void j1850_asynch_exc::reset_loop(void) {
  tmp_byte = 0x00;
  bites = 0;
  accept_bytes = 0;
  position = 0;
}

unsigned char j1850_asynch_exc::crc(volatile unsigned char *msg_buf, volatile int nbytes) {
  unsigned char crc = 0xFF;
  while (nbytes--){
    crc ^= *msg_buf++;
    for (int i = 0; i < 8; i++){
      crc = crc & 0x80 ? (crc << 1) ^ 0x1D : crc << 1;
    }
  }
  crc ^= 0xFF;
  return crc;
}

void j1850_asynch_exc::__passive(long tmp) {
  if (position == 2) {
    if ((tmp > RX_SHORT_MIN) && (tmp < RX_SHORT_MAX)) {
      tmp_byte |= 1;
    }
    bites++;

    if (bites == 8) {
      if (accept_bytes > 11) {
        message = ERROR_MESSAGE_TO_LONG;
        reset_loop();
        position = 1;
        return;
      }
      accept_buf[accept_bytes++] = tmp_byte;
      if (accept_bytes > 2) {
        if (crc(accept_buf, accept_bytes - 1) == tmp_byte) {
          message = MESSAGE_ACCEPT_OK;
          bytes = accept_bytes;
          reset_loop();
        }
      }
      tmp_byte = 0x00;
      bites = 0;
    } else {
      tmp_byte <<= 1;
    }
    return;
  }
  if (position == 1) {
    if ((tmp < RX_SOF_MAX) && (tmp > RX_SOF_MIN)) {
      position = 2;
    } else {
      message = ERROR_SYMBOL_WAS_NOT_SOF;
      reset_loop();
    }
  }
}

void j1850_asynch_exc::__active(long tmp) {
  if (position == 0) {
    position = 1;
    return;
  }
  if (position == 2) {
    if (tmp > RX_EOF_MIN) {
      reset_loop();
      position = 1;
      message = ERROR_ACCEPT_CRC;
      return;
    }
    
    if ((tmp > RX_LONG_MIN) && (tmp < RX_LONG_MAX)) {
      tmp_byte |= 1;
    }
    
    bites++;
    tmp_byte <<= 1;
  }
}

void j1850_asynch_exc::__separator(void) {
  if (interrupt_locking) return;
  
  bool pos = digitalRead(in_pin);

  if (pos) {
    __passive(read_timer());
  } else {
    __active(read_timer());
  }
  start_timer();
}

void j1850_asynch_exc::reset_accept(void) {
  for (uint8_t i = 0; i < accept_bytes; i++) {
    accept_buf[i] = 0x00;
  }
  reset_loop();
  bytes = 0;
  message = 0;
}

void j1850_asynch_exc::start_timer(void) {
	time_tmp = micros();
}

unsigned long j1850_asynch_exc::read_timer(void) {
  unsigned long time_data  = micros();
  if (time_data <  time_tmp){
    return 1000 - (time_tmp - time_data);
  }else{
    return time_data - time_tmp;
  }
}
