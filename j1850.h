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

#define MESSEGE_SEND_OK 1
#define MESSEGE_ACCEPT_OK 2
#define ERROR_MESSEGE_TO_LONG 3
#define ERROR_NO_RESPONDS_WITHIN_100US 4
#define ERROR_ON_SOF_TIMEOUT 5
#define ERROR_SIMBOLE_WAS_NOT_SOF 6
#define ERROR_SIMBOLE_WAS_NOT_SHORT 7
#define ERROR_ACCEPT_CRC 8

class j1850
{
  private:
	int in_pin;
	int out_pin;
	bool if_init = false;
	bool review;
	unsigned long time_tmp = 0;	
	int mode;
	byte *rx_msg_buf;
	int rx_nbyte;
	byte *tx_msg_buf;
	int tx_nbyte;
	void start_timer(void);
	unsigned long read_timer(void);
	void monitor(void);
	void sendToUART(const char *, int, byte *);
	void tests(void);
	void active(void);
	void passive(void);
	bool is_active(void);
	byte crc(byte *, int);
	bool recv_msg(byte *);
	bool send_msg(byte *, int);
  public:
	int message;
	void init(int, int, bool monitor_ = false);
	bool accept(byte *, bool crt = false);
	bool send(byte *, int);
	bool easy_send(int size, ...);
};
#endif