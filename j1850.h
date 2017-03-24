#ifndef J1850
#define J1850

#include <Arduino.h>
#include "timer.h"

#define us2cnt(us) ((unsigned int)((unsigned long)(us) / (1000000L / (float)((unsigned long)F_CPU / 64L))))

#define WAIT_100us	us2cnt(100)

#define TX_SHORT	us2cnt(64)		// Short pulse nominal time
#define TX_LONG		us2cnt(128)		// Long pulse nominal time
#define TX_SOF		us2cnt(200)		// Start Of Frame nominal time
#define TX_EOD		us2cnt(200)		// End Of Data nominal time
#define TX_EOF		us2cnt(280)		// End Of Frame nominal time
#define TX_BRK		us2cnt(300)		// Break nominal time
#define TX_IFS		us2cnt(300)		// Inter Frame Separation nominal time

#define TX_IFR_SHORT_CRC	us2cnt(64)	// short In Frame Respond, IFR contain CRC
#define TX_IFR_LONG_NOCRC	us2cnt(128)	// long In Frame Respond, IFR contain no CRC


#define RX_SHORT_MIN	us2cnt(34)	// minimum short pulse time
#define RX_SHORT_MAX	us2cnt(96)	// maximum short pulse time
#define RX_LONG_MIN		us2cnt(96)	// minimum long pulse time
#define RX_LONG_MAX		us2cnt(163)	// maximum long pulse time
#define RX_SOF_MIN		us2cnt(163)	// minimum start of frame time
#define RX_SOF_MAX		us2cnt(239)	// maximum start of frame time
#define RX_EOD_MIN		us2cnt(163)	// minimum end of data time
#define RX_EOD_MAX		us2cnt(239)	// maximum end of data time
#define RX_EOF_MIN		us2cnt(239)	// minimum end of frame time, ends at minimum IFS
#define RX_BRK_MIN		us2cnt(239)	// minimum break time
#define RX_IFS_MIN		us2cnt(280)	// minimum inter frame separation time, ends at next SOF

#define RX_IFR_SHORT_MIN	us2cnt(34)		// minimum short in frame respond pulse time
#define RX_IFR_SHORT_MAX	us2cnt(96)		// maximum short in frame respond pulse time
#define RX_IFR_LONG_MIN		us2cnt(96)		// minimum long in frame respond pulse time
#define RX_IFR_LONG_MAX		us2cnt(163)		// maximum long in frame respond pulse time


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
    int mode;
	
	byte *rx_msg_buf;
	int rx_nbyte;
	byte *tx_msg_buf;
	int tx_nbyte;

	void monitor(void);
	void sendToUART(const char*, int, byte*);
	void wait_idle(void);
	void tests(void);
	
	void active(void);
	void passive(void);
	bool is_active(void);
	byte crc(byte*, int);
	bool recv_msg(byte*);
	bool send_msg(byte*, int);
public:
	int messege;
	void init(int, int, bool monitor_ = false);
	bool accept(byte*, bool crt = false);
	bool send(byte*, int);
};

#endif