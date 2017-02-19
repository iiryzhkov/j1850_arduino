#include <Arduino.h>

void start_timer(void) {
	TCNT0 = 0;
	TCCR0B = (0 << CS12) | (1 << CS11) | (1 << CS10);
}

void stop_timer(void) {
	TCCR0B = (0 << CS12) | (0 << CS11) | (0 << CS10);
}

int read_timer(void){
	return TCNT0;
}