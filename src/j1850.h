#ifndef J1850
#define J1850

#include <Arduino.h>
#include "j1850_asynch_exc.h"

class j1850: public j1850_asynch_exc {
  private:
		bool if_init = false;
		bool asy = false;
		int out_pin = 0;
		int monitoring_mode = 0;
		byte *rx_msg_buf;
		byte *tx_msg_buf;
		Print* pr;

		void monitor(void);
		void sendToUART(const char *, int, byte *);
		void tests(void);
		void active(void);
		void passive(void);
		bool is_active(void);
		bool recv_msg(byte *);
		bool send_msg(byte *, int);
  public:
		void init(int, int, bool asy_ = false, Print* pr_ = &Serial);
		void set_monitoring(int mod = 1);
		bool accept(byte *, bool crt = false);
		bool send(byte *, int);
		bool easy_send(int size, ...);

		int rx_nbyte = 0;
		int tx_nbyte = 0;
};
#endif