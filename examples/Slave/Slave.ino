#include <j1850_distributed.h>

//Create an instance of the class
j1850_slave j;

bool filter(byte *data, int len)
{
  // If the message length is less than 4 and the first character is not 0x8D,
  // then return false
  if ((len < 4) or (data[0] != 0x8D)){
    return false;
  }
  return true;
}

void setup() {
  // LED to indicate the buffer
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(2000000);
  //Initialize. RX pin, TX pin
  j.init(10, 11, &Serial);
  //Set the filter
  j.set_filter(&filter);
  j.set_monitoring(0);
}

void loop() {
  if (Serial.available() > 0) j.set_monitoring(Serial.parseInt());
  j.loop();
  // Turn on the LED if the buffer is not empty
  if (j.len_buffer()){
    digitalWrite(LED_BUILTIN, HIGH);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
}
