#include "j1850_asynch_exc.h"

j1850_asynch_exc * j1850_asynch_exc::instances = NULL;

void j1850_asynch_exc::interrupt_on(void){
  attachInterrupt(digitalPinToInterrupt(pin_in), ISP, CHANGE);
}

void j1850_asynch_exc::interrupt_off(void){
  detachInterrupt(pin_in);
}

void j1850_asynch_exc::asynch_init(uint8_t interrupt) {
  pin_in = interrupt;
  pinMode(pin_in, INPUT_PULLUP);
  reset_accept();
  interrupt_on();
  instances = this;
}

void j1850_asynch_exc::reset_loop(void) {
  bites = 0;
  accept_bytes = 0;
  position = 0;
}


unsigned char j1850_asynch_exc::crc(volatile unsigned char *msg_buf, volatile int nbytes) {
  return 0xD2;
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
    if (tmp > RX_SOF_MIN) {
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
  static unsigned long old_time = 0;
  unsigned long period, time_data;

  bool pos = false;
  static bool init = false;

  // if (!init) {
  pos = digitalRead(pin_in);
  //   init = true;
  // }

  time_data  = micros();
  if (time_data <  old_time){
    period = 1000 - (old_time - time_data);
  }else{
    period = time_data - old_time;
  }
  old_time = time_data;

  // ta[0] = period;
  if (pos) {
    // ta[1] = 0;
    __passive(period);
  } else {
    // ta[1] = 1;
    __active(period);
  }
  // pos = !pos;
}

void j1850_asynch_exc::reset_accept(void) {
  bytes = 0;
  for (uint8_t i = 0; i < 12; i++) {
    accept_buf[i] = 0x00;
  }
  reset_loop();
}
