#include "j1850.h"
#include <Arduino.h>

void j1850::init(int in_pin_, int out_pin_, bool review_) {
	out_pin = out_pin_;
	in_pin = in_pin_;
	review = review_;

	if (review_) {
		Serial.begin(9600);
		mode = 1;
	}

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

	if (review) {
		if (Serial.available() >= 2)
			mode = Serial.parseInt();
		monitor();
	}
	return f;
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

	msg_buf[nbytes] = crc(msg_buf, nbytes);
	nbytes++;

	bool f = send_msg(msg_buf, nbytes);

	if (review)
		monitor();
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
		message = ERROR_SIMBOLE_WAS_NOT_SOF;
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
					message = MESSEGE_ACCEPT_OK;
					return true;
				}
			}

			bit_state = is_active();
			long tcnt1_buf = read_timer();
			start_timer();

			if (tcnt1_buf < RX_SHORT_MIN) {
				message = ERROR_SIMBOLE_WAS_NOT_SHORT;
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
	message = MESSEGE_ACCEPT_OK;
	return true;
}

bool j1850::send_msg(byte *msg_buf, int nbytes) {
	int nbits;
	byte temp_byte;
	tx_msg_buf = msg_buf;
	tx_nbyte = nbytes;

	if (nbytes > 12) {
		message = ERROR_MESSEGE_TO_LONG;
		return false;
	}

	start_timer();
	while (read_timer() < RX_IFS_MIN) {
		if (is_active())
			start_timer();
	}

	active();
	delayMicroseconds(TX_SOF);
	do {
		temp_byte = *msg_buf;
		nbits = 8;
		while (nbits--) {
			if (nbits & 1) {
				passive();
				delayMicroseconds((temp_byte & 0x80) ? TX_LONG : TX_SHORT);
			} else {
				active();
				delayMicroseconds((temp_byte & 0x80) ? TX_SHORT : TX_LONG);
			}
			temp_byte <<= 1;
		}
		++msg_buf;
	} while (--nbytes);

	passive();
	delayMicroseconds(TX_EOF);
	message = MESSEGE_SEND_OK;
	return true;
}

void j1850::monitor(void) {
	static byte old_messege;
	switch (mode) {
		//tests
		case 5:
			tests();
			mode = 1; //default mode
			break;

		//RX
		case 4:
			if (MESSEGE_ACCEPT_OK == message)
				sendToUART("RX: ", rx_nbyte, rx_msg_buf);
			break;

		//TX
		case 3:
			if (MESSEGE_SEND_OK == message)
				sendToUART("TX: ", tx_nbyte, tx_msg_buf);
			break;

		//status codes
		case 2:
			if (old_messege != message) {
				Serial.println(message);
				old_messege = message;
			}
			break;

		//RX\TX
		case 1:
			if (MESSEGE_SEND_OK == message)
				sendToUART("TX: ", tx_nbyte, tx_msg_buf);
			if (MESSEGE_ACCEPT_OK == message)
				sendToUART("RX: ", rx_nbyte, rx_msg_buf);
			break;

		// filtering messages by the first byte
		default:
			if ((MESSEGE_ACCEPT_OK == message) && (mode == rx_msg_buf[0]))
				sendToUART("RX: ", rx_nbyte, rx_msg_buf);
			break;
	}
}

void j1850::sendToUART(const char *header, int rx_nbyte, byte *msg_buf) {
	Serial.print(header);
	for (int i = 0; i < rx_nbyte; i++) {
		if (msg_buf[i] < 0x10)
			Serial.print(0);

		Serial.print(msg_buf[i], HEX);

		if (i == (rx_nbyte - 1)) {
			Serial.print("\n");
		} else {
			Serial.print(" ");
		}
	}
}

void j1850::tests(void) {
	char fail[] = "Test failure!\n";
	char ok[] = "Test succes!\n";
	//тест i\o
	Serial.print("----Start I/O test----\n");
	if (!is_active()) {
		active();
		if (is_active()) {
			Serial.print(ok);
		} else {
			Serial.print(fail);
		}
		passive();
	} else {
		Serial.print(fail);
	}
	Serial.print("----End I/O test----\n\n");
}

void j1850::active(void) {
	digitalWrite(out_pin, LOW);
}

void j1850::passive(void) {
	digitalWrite(out_pin, HIGH);
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