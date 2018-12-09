#ifndef J1850
#define J1850

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
#define ERROR_SEND_COLLISION 9

#define ATTEMPT_TO_SEND 5
#define TOLERANCE 2 // 2 - 1/2 time

class j1850 {
  private:
	bool if_init = false;
	int in_pin = 0;
	int out_pin = 0;
	unsigned long time_tmp = 0;

	void sendToUART(const char *, int, byte *);
	void tests(void);
	bool active(int _time = 0);
	bool passive(int _time = 0);
	bool is_active(void);
	bool recv_msg(byte *);
	bool send_msg(byte *, int);
	byte crc(byte *, int);
  protected:
	void monitor(void);
	void start_timer(void);
	int read_timer(void);
  	int monitoring_mode = 0;
	Print* pr;
	byte *rx_msg_buf;
	byte *tx_msg_buf;
  public:
	void init(int, int, Print* pr_ = &Serial);
	void set_monitoring(int mod = 1);
	bool accept(byte *, bool crt = false);
	bool send(byte *, int);
	bool easy_send(int size, ...);

	volatile int message;
	int rx_nbyte = 0;
	int tx_nbyte = 0;
};
#endif