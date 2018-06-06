#ifndef J1850
#define J1850

#include <Arduino.h>
#include "j1850_asynch_exc.h"

class j1850: public j1850_asynch_exc {
  private:
    bool if_init = false;
    bool asy = false;
    uint8_t out_pin = 0;
    uint8_t monitoring_mode = 0;
    byte *rx_msg_buf;
    volatile byte *tx_msg_buf;
    Print* pr;

    void monitor(void);
    void tests(void);
    void active(void);
    void passive(void);
    bool is_active(void);
    bool recv_msg(byte *);
    bool send_msg(volatile byte *, volatile uint8_t);
  public:
    uint8_t rx_nbyte = 0;
    uint8_t tx_nbyte = 0;
    void sendToUART(const char *, volatile uint8_t, volatile byte *);
    void init(uint8_t, uint8_t, bool asy_ = false, Print* pr_ = &Serial);
    void set_monitoring(uint8_t mod = 1);
    bool accept(byte *, bool crt = false);
    bool send(volatile byte*, volatile uint8_t);
    bool easy_send(uint8_t size, ...);
};
#endif