#ifndef J1850_ASY_EXE
#define J1850_ASY_EXE

#include <Arduino.h>

#define WAIT_100us 100
#define TX_SHORT 64
#define TX_LONG 128
#define TX_SOF 200
#define TX_EOF 280
#define RX_SHORT_MIN 34
#define RX_SHORT_MAX 96
#define RX_LONG_MIN 96
#define RX_LONG_MAX 163
#define RX_SOF_MIN 163
#define RX_SOF_MAX 239
#define RX_EOD_MIN 163
#define RX_IFS_MIN 280

#define MESSAGE_SEND_OK 1
#define MESSAGE_ACCEPT_OK 2
#define ERROR_MESSAGE_TO_LONG 3
#define ERROR_NO_RESPONDS_WITHIN_100US 4
#define ERROR_ON_SOF_TIMEOUT 5
#define ERROR_SYMBOL_WAS_NOT_SOF 6
#define ERROR_SYMBOL_WAS_NOT_SHORT 7
#define ERROR_ACCEPT_CRC 8

class j1850_asynch_exc {
  static j1850_asynch_exc * instances;
  static void ISP() {
    j1850_asynch_exc::instances->__separator();
  }

  public:
    volatile uint8_t bytes        = 0;//12
    volatile uint8_t message      = 0;//8
    volatile byte accept_buf[12]  = {0x00};
    void asynch_init(uint8_t interrupt);
    void reset_accept(void);
    void __separator(void);
  protected:
    uint8_t in_pin = 0;
    void interrupt_on(void);
    void interrupt_off(void);
    unsigned char crc(volatile unsigned char *msg_buf, volatile int nbytes);
  private:
    volatile uint8_t position     = 0;//2
    volatile uint8_t bites        = 0;//8
    volatile uint8_t accept_bytes = 0;//12
    volatile byte tmp_byte        = 0x00;
    void __passive(long tmp);
    void __active(long tmp);
    void reset_loop(void);
};
#endif