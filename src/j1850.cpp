#include "j1850.h"
#include <Arduino.h>
#include <stdarg.h>

void j1850::init(int in_pin_, int out_pin_, Print* pr_) {
	out_pin = out_pin_;
	in_pin = in_pin_;
	pr = pr_;

	pinMode(in_pin_, INPUT_PULLUP);
	pinMode(out_pin_, OUTPUT);
	passive();
	if_init = true;
}

bool j1850::accept(byte *msg_buf, bool crt_c) {
	if (!if_init)
		return false;

	bool f = recv_msg(msg_buf);

	if ((crt_c) && (f)) {
		if (msg_buf[rx_nbyte - 1] != crc(msg_buf, rx_nbyte - 1)) {
			f = false;
			message = ERROR_ACCEPT_CRC;
		}
	}

	if (monitoring_mode > 0)
		monitor();

	return f;
}

void j1850::set_monitoring(int mode_){
	if((mode_ > 5) or (mode_ < 0)){
		pr->println("The mode should be from 0 to 5");
		return;
	}
	monitoring_mode = mode_;
}

bool j1850::easy_send(int size, ...) {
	if (size == 0)
		return false;

	byte *buffer = new byte[size];
	bool result = false;
	va_list ap;

	va_start(ap, size);

	for (int i = 0; i < size; i++)
		buffer[i] = va_arg(ap, int);

	va_end(ap);

	result = send(buffer, size);
	delete[] buffer;
	return result;
}

bool j1850::send(byte *msg_buf, int nbytes) {
	if (!if_init)
		return false;
	bool f = false;
	msg_buf[nbytes] = crc(msg_buf, nbytes);
	nbytes++;
	for (int i=0; i<ATTEMPT_TO_SEND; i++){
		if(send_msg(msg_buf, nbytes)){
			f = true;
			break;
		}else if (message != ERROR_SEND_COLLISION){
			f = false;
			break;
		}
		delay(1);
	}

	if (monitoring_mode > 0){
		monitor();
	}
	return f;
}

bool j1850::recv_msg(byte *msg_buf) {
	int nbits, nbytes;
	bool bit_state;
	rx_msg_buf = msg_buf;

	start_timer();
	while (!is_active()) {
		if (read_timer() > WAIT_100us) {
			message = ERROR_NO_RESPONDS_WITHIN_100US;
			return false;
		}
	}

	start_timer();
	while (is_active()) {
		if (read_timer() > RX_SOF_MAX) {
			message = ERROR_ON_SOF_TIMEOUT;
			return false;
		}
	}

	if (read_timer() < RX_SOF_MIN) {
		message = ERROR_SYMBOL_WAS_NOT_SOF;
		return false;
	}

	bit_state = is_active();
	start_timer();

	for (nbytes = 0; nbytes < 12; ++nbytes) {
		nbits = 8;
		do {
			*msg_buf <<= 1;
			while (is_active() == bit_state) {
				if (read_timer() > RX_EOD_MIN) {
					rx_nbyte = nbytes;
					message = MESSAGE_ACCEPT_OK;
					return true;
				}
			}

			bit_state = is_active();
			long tcnt1_buf = read_timer();
			start_timer();

			if (tcnt1_buf < RX_SHORT_MIN) {
				message = ERROR_SYMBOL_WAS_NOT_SHORT;
				return false;
			}

			if ((tcnt1_buf < RX_SHORT_MAX) && !is_active())
				*msg_buf |= 1;

			if ((tcnt1_buf > RX_LONG_MIN) && (tcnt1_buf < RX_LONG_MAX) && is_active())
				*msg_buf |= 1;

		} while (--nbits);
		++msg_buf;
	}

	rx_nbyte = nbytes;
	message = MESSAGE_ACCEPT_OK;
	return true;
}

bool j1850::send_msg(byte *msg_buf, int nbytes) {
	int nbits;
	byte temp_byte;
	tx_msg_buf = msg_buf;
	tx_nbyte = nbytes;

	if (nbytes > 12) {
		message = ERROR_MESSAGE_TO_LONG;
		return false;
	}

	start_timer();
	while (read_timer() < RX_IFS_MIN) {
		if (is_active())
			start_timer();
	}

	if (!active(TX_SOF)) return false;
	do {
		temp_byte = *msg_buf;
		nbits = 8;
		while (nbits--) {
			if (nbits & 1) {
				if(!passive((temp_byte & 0x80) ? TX_LONG : TX_SHORT)) return false;
			} else {
				if(!active((temp_byte & 0x80) ? TX_SHORT : TX_LONG)) return false;
			}
			temp_byte <<= 1;
		}
		++msg_buf;
	} while (--nbytes);

	if(!passive(TX_EOF)) return false;
	message = MESSAGE_SEND_OK;
	return true;
}

void j1850::monitor(void) {
	static int old_message;
	
	switch (monitoring_mode) {
		//tests
		case 5:
			tests();
			monitoring_mode = 1; //default mode
			break;

		//RX
		case 4:
			if (MESSAGE_ACCEPT_OK == message)
				sendToUART("RX: ", rx_nbyte, rx_msg_buf);
			break;

		//TX
		case 3:
			if (MESSAGE_SEND_OK == message)
				sendToUART("TX: ", tx_nbyte, tx_msg_buf);
			break;

		//status codes
		case 2:
			if (old_message != message) {
				pr->println(message);
				old_message = message;
			}
			break;

		//RX\TX
		case 1:
			if (MESSAGE_SEND_OK == message)
				sendToUART("TX: ", tx_nbyte, tx_msg_buf);
			if (MESSAGE_ACCEPT_OK == message)
				sendToUART("RX: ", rx_nbyte, rx_msg_buf);
			break;

		default:
			break;
	}
}

void j1850::sendToUART(const char *header, int nbyte, byte *msg_buf) {
	pr->print(header);
	for (int i = 0; i < nbyte; i++) {
		if (msg_buf[i] < 0x10)
			pr->print(0);

		pr->print(msg_buf[i], HEX);
		pr->print(" ");
	}
	pr->print("\n");
}

void j1850::tests(void) {
	char fail[] = "Test failure!\n";
	char ok[] = "Test success!\n";
	//тест i\o
	pr->print("----Start I/O test----\n");
	if (!is_active()) {
		active();
		if (is_active()) {
			pr->print(ok);
		} else {
			pr->print(fail);
		}
		passive();
	} else {
		pr->print(fail);
	}
	pr->print("----End I/O test----\n\n");
}

bool j1850::active(int _time) {
	if (is_active()){
		passive();
		message = ERROR_SEND_COLLISION;
		return false;
	}
	start_timer();
	digitalWrite(out_pin, LOW);
	if(!check_io_timing(_time)){
		passive();
		return false;
	}
	return true;
}

bool j1850::passive(int _time) {
	if (!is_active()){
		message = ERROR_SEND_COLLISION;
		return false;
	}
	start_timer();
	digitalWrite(out_pin, HIGH);
	return check_io_timing(_time);
}

bool j1850::check_io_timing(int _time){
	if (_time == 0) {
		return true;
	}
	delayMicroseconds(_time);
	long len_work = read_timer();
	if ((len_work > _time + _time / TOLERANCE) || (len_work < _time - _time / TOLERANCE)){
		message = ERROR_SEND_COLLISION;
		return false;
	}
	return true;
}

bool j1850::is_active(void) {
	return !digitalRead(in_pin);
}

byte j1850::crc(byte *msg_buf, int nbytes) {
	byte crc = 0xFF;
	while (nbytes--) {
		crc ^= *msg_buf++;
		for (int i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x1D : crc << 1;
	}
	crc ^= 0xFF;
	return crc;
}

void j1850::start_timer(void) {
	time_tmp = micros();
}

int j1850::read_timer(void) {
	return micros() - time_tmp;
}