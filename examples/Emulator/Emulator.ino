#include <j1850.h>
//Emulator for car radio chrysler
//Create an instance of the class

#define PASS 100
#define RADIO_OFF 101
#define DARIO_AM  102
#define DARIO_FM  103
#define DARIO_CD  104
#define DARIO_CC  105
#define DISPLAY_OFF   0
#define DISPLAY_SCAN  1
#define DISPLAY_RND   2
#define DISPLAY_WAIT  3
#define DISPLAY_NMAG  4
#define DISPLAY_NDISC 5
#define DISPLAY_SPLIT 6
#define DISPLAY_DT    7
#define DISPLAY_DTT   8
#define DISPLAY_DTZZ  9
#define DISPLAY_DTPT  10
#define DISPLAY_VOID  11

j1850 j;

void setup() {
    Serial.begin(9600);
    //Initialize. RX pin, TX pin
    j.init(3, 4, &Serial);
}

void loop() {
  static bool on_cdc = false;
  if (Serial.available() > 0) j.set_monitoring(Serial.parseInt());
  int the_code = accept();
  // no events
  if (the_code == PASS) {
    return;
  }
  
  //off
  if (the_code == RADIO_OFF) {
    cd_ch_disp(DISPLAY_OFF);
    on_cdc = false;
    return;
  }
  //activate
  if ((the_code > RADIO_OFF) && (the_code < DARIO_CC)) {
    cd_ch_disp(DISPLAY_WAIT);
    on_cdc = false;
    return;
  }
  // cd changer mode
  if (the_code == DARIO_CC) {
    if (!on_cdc)
    {
      j.easy_send(5, 0x8D, 0x92, 0xC0, 0x00, 0x00);
      j.easy_send(5, 0x8D, 0x92, 0xE1, 0x01, 0x03);
      j.easy_send(5, 0x8D, 0x92, 0xE3, 0x01, 0x99);
      on_cdc = true;
    }
    cd_ch_disp(DISPLAY_DTT);
    return;
  }
}

int accept() {
  int the_code = PASS;
  byte rxmsg_buf[12];
  if (!j.accept(rxmsg_buf, true)) {
    return the_code;
  }
  if ((rxmsg_buf[0] == 0x8D) && (rxmsg_buf[1] == 0x0F)) {
    if (rxmsg_buf[2] < 0x05) {
      return RADIO_OFF;
    }
    switch (rxmsg_buf[2]) {
      case 0x20:
        the_code = DARIO_AM;
        break;
      case 0x21:
        the_code = DARIO_FM;
        break;
      case 0x23:
        the_code = DARIO_CD;
        break;
      case 0x24:
        the_code = DARIO_CC;
        break;
      default:
        break;
    }
  }
  return the_code;
};

void cd_ch_disp(int mode_) {
  int mode = 0x00;
  switch (mode_) {
    case DISPLAY_OFF:
      mode = 0x00;
      break;
    case DISPLAY_SCAN:
      mode = 0x02;
      break;
    case DISPLAY_RND:
      mode = 0x04;
      break;
    case DISPLAY_WAIT:
      mode = 0x10;
      break;
    case DISPLAY_NMAG:
      mode = 0x30;
      break;
    case DISPLAY_NDISC:
      mode = 0x60;
      break;
    case DISPLAY_SPLIT:
      mode = 0x70;
      break;
    case DISPLAY_DT:
      mode = 0x80;
      break;
    case DISPLAY_DTT:
      mode = 0x90;
      break;
    case DISPLAY_DTZZ:
      mode = 0xA0;
      break;
    case DISPLAY_DTPT:
      mode = 0xB0;
      break;
    case DISPLAY_VOID:
      mode = 0x0B;
      break;
    default:
      break;
  }
  j.easy_send(5, 0x8D, 0x93, 0x01, 0x01, mode);
};
