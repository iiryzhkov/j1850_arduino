#include "j1850.h"
#include <Arduino.h>
#include "timer.h"

void j1850::init(int in_pin_, int out_pin_, bool review_)
{
	// инициализация параметров
	out_pin = out_pin_;
	in_pin = in_pin_;
    review = review_;
	
	//если включен мониторинг, активируем Serial
    if(review_){
		Serial.begin(9600);
		mode = 1;
	} 
	
	//инициализация I\O
	pinMode(in_pin_, INPUT_PULLUP);
	pinMode(out_pin_, OUTPUT);
	
	//переключаем порт передачи в положение 0
	passive();
	
	//инициализация пройдена
	if_init = true;
}

bool j1850::accept(byte* msg_buf, bool crt_c)
{
	//если не выполненна инициализация - уходим
	if(!if_init) return false;
	
	//прием данных
	bool f = recv_msg(msg_buf);
	
	//вызывается при положительном результате 
	//приема и при включении данной опции
	if((crt_c) && ( f )){
		if(msg_buf[rx_nbyte - 1] != crc(msg_buf, rx_nbyte - 1)){
			f = false;
			messege = ERROR_ACCEPT_CRC;
		}
	}
	
	//мониторинг
	//считывание команд, а так же сам мониторинг
	if(review){
		if(Serial.available() >= 2) {
			mode = Serial.parseInt();
		}
		monitor();
	}
	
	
	return f;
}

bool j1850::send(byte *msg_buf, int nbytes)
{	
	//если не выполненна инициализация - уходим
	if(!if_init) return false;
	
	//Добавляем к массиву передаваемых данных crc сумму
	msg_buf[nbytes] = crc(msg_buf, nbytes);
	nbytes++;
	
	//передача данных
	bool f = send_msg(msg_buf, nbytes);
	
	//если надо, внключаем мониторинг
	if(review) monitor();
	
	return f;
}

bool j1850::recv_msg(byte *msg_buf)
{
	int nbits, nbytes;
	bool bit_state;
	rx_msg_buf = msg_buf;

	start_timer();
	while (!is_active())
	{
		if (read_timer() > WAIT_100us)
		{
			stop_timer();
			messege = ERROR_NO_RESPONDS_WITHIN_100US;
			return false;
		}
	}
	
	start_timer();
	while (is_active())
	{
		if (read_timer() > RX_SOF_MAX) {
			messege = ERROR_ON_SOF_TIMEOUT;
			return false;
		}
	}

	stop_timer();
	if (read_timer() < RX_SOF_MIN) {
		messege = ERROR_SIMBOLE_WAS_NOT_SOF;
		return false;
	}
	
	bit_state = is_active();
	start_timer();
	for (nbytes = 0; nbytes < 12; ++nbytes)
	{
		nbits = 8;
		do
		{
			*msg_buf <<= 1;
			while (is_active() == bit_state)
			{
				if (read_timer() > RX_EOD_MIN)
				{
					stop_timer();
					rx_nbyte = nbytes;
					messege = MESSEGE_ACCEPT_OK;
					return true;
				}
			}

			bit_state = is_active();
			long tcnt1_buf = read_timer();
			start_timer();
			if (tcnt1_buf < RX_SHORT_MIN) {
				stop_timer();
				messege = ERROR_SIMBOLE_WAS_NOT_SHORT;
				return false;
			}

			if ((tcnt1_buf < RX_SHORT_MAX) && !is_active())
				*msg_buf |= 1;

			if ((tcnt1_buf > RX_LONG_MIN) && (tcnt1_buf < RX_LONG_MAX) && is_active())
				*msg_buf |= 1;

		} while (--nbits);

		++msg_buf;

	}

	stop_timer();
	rx_nbyte = nbytes;
	messege = MESSEGE_ACCEPT_OK;
	return true;
}

bool j1850::send_msg(byte *msg_buf, int nbytes)
{
	int nbits, delay;
	byte temp_byte;

	tx_msg_buf = msg_buf;
	tx_nbyte = nbytes;

	if (nbytes > 12) {
		messege = ERROR_MESSEGE_TO_LONG;
		return false;
	}
	wait_idle();

	start_timer();
	active();

	while (read_timer() < TX_SOF);
	do
	{
		temp_byte = *msg_buf;
		nbits = 8;
		while (nbits--)
		{
			if (nbits & 1)
			{
				passive();
				start_timer();
				delay = (temp_byte & 0x80) ? TX_LONG : TX_SHORT;
				while (read_timer() < delay);
			}
			else
			{
				active();
				start_timer();
				delay = (temp_byte & 0x80) ? TX_SHORT : TX_LONG;
				while (read_timer() < delay);
			}
			temp_byte <<= 1;
		}
		++msg_buf;
	} while (--nbytes);

	passive();
	start_timer();
	while (read_timer() <= TX_EOF);
	stop_timer();
	messege = MESSEGE_SEND_OK;
	return true;
}

void j1850::monitor(void)
{
	static byte old_messege;
	switch(mode){
		//режим тестирования
		case 5:
		tests();
		mode = 1; //default mode
		break;
		
		//режим - только RX
		case 4:
		if (MESSEGE_ACCEPT_OK == messege) {
			Serial.print("RX: ");
			for (int i = 0; i < rx_nbyte; i++)
			{
				Serial.print(rx_msg_buf[i], HEX);
				Serial.print(" ");
			}
			Serial.print("\n");
		}
		break;
		
		//режим - только TX
		case 3:
		if (MESSEGE_SEND_OK == messege) {
			Serial.print("TX: ");
			for (int i = 0; i < tx_nbyte; i++)
			{
				Serial.print(tx_msg_buf[i], HEX);
				Serial.print(" ");
			}
			Serial.print("\n");
		}
		break;
		
		//режим - только системные события
		case 2:
		if (old_messege != messege) {
			Serial.println(messege);
			old_messege = messege;
		}
		break;
		
		//режим - RX\TX
		case 1:
		if (MESSEGE_SEND_OK == messege) {
			Serial.print("TX: ");
			for (int i = 0; i < tx_nbyte; i++)
			{
				Serial.print(tx_msg_buf[i], HEX);
				Serial.print(" ");
			}
			Serial.print("\n");
		}
		if (MESSEGE_ACCEPT_OK == messege) {
			Serial.print("RX: ");
			for (int i = 0; i < rx_nbyte; i++)
			{
				Serial.print(rx_msg_buf[i], HEX);
				Serial.print(" ");
			}
			Serial.print("\n");
		}
		break;
		
		//режим выбора сообщений по заголовку
		default:
		if((MESSEGE_ACCEPT_OK == messege) && (mode == rx_msg_buf[0])){
			Serial.print("RX: ");
			for (int i = 0; i < rx_nbyte; i++)
			{
				Serial.print(rx_msg_buf[i], HEX);
				Serial.print(" ");
			}
			Serial.print("\n");
		}	
		break;
	}
	
}
void j1850::tests(void){
		char fail[ ] = "Test failure!\n";
		char ok[ ] = "Test succes!\n";
		//тест счетчика
		Serial.print("----Start timer test----\n");
		Serial.print("F_CPU = ");
		Serial.print(F_CPU);
		Serial.print("Hz\n100us = ");
		Serial.print(WAIT_100us);
		Serial.print("cycles\n\n");

		for(int i = 10; i<110; i+=10){
			start_timer();
			delayMicroseconds(i);
			stop_timer();
			
			Serial.print(i);
			Serial.print("ms ~ ");
			Serial.print(read_timer());
			Serial.print(" cycles\n");
		}
		if(read_timer() == WAIT_100us){
			Serial.print(ok);
		}else{
			Serial.print(fail);
		}
		Serial.print("----End timer test----\n\n");
		
		//тест i\o
		Serial.print("----Start I/O test----\n");
		if(!is_active()){
			active();
			if(is_active()){
				Serial.print(ok);
			}else{
				Serial.print(fail);
			}
			passive();
		}else{
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

void j1850::wait_idle(void)
{
	start_timer();
	while (read_timer() < RX_IFS_MIN)
	{
		if (is_active()) start_timer();
	}
}

byte j1850::crc(byte *msg_buf, int nbytes)
{
	int i;
    	byte crc = 0xFF;
	
    	while (nbytes--)
    	{
        	crc ^= *msg_buf++;
 
        	for (i = 0; i < 8; i++)
            		crc = crc & 0x80 ? (crc << 1) ^ 0x1D : crc << 1;
    	}
    	crc ^= 0xFF;
	return crc;
}
