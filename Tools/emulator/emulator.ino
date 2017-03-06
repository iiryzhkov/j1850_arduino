
int len_srts[] = {4, 5, 2, 3, 6};

byte str0[] = { 0x01, 0x02, 0x02, 0x03};
byte str1[] = { 0xE1, 0xB2, 0x35, 0xFF, 0x02};
byte str2[] = { 0xE1, 0xB3};
byte str3[] = { 0xA1, 0xF2, 0x35};
byte str4[] = { 0x01, 0x02, 0x02, 0x03, 0xFF, 0x02};

char her[] = "RX: ";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void sendToUART(char* header, int rx_nbyte, byte *msg_buf) {
  Serial.print(header);
  for (int i = 0; i < rx_nbyte; i++) {
    Serial.print(msg_buf[i], HEX);
    if(i == (rx_nbyte - 1)){
      Serial.print("\n");
    }else{
      Serial.print(" ");
    }
  }
}

void RandomMass() {
  byte tmp[6];
  int len_mass = random(2, 7);
  for (int i = 0; i < len_mass; i++) {
    tmp[i] = (byte)random(0, 254);
  }
  sendToUART(her, len_mass, tmp);
}

void PrintRandomStr(int f = 0) {
  if (f == 0) {
    f = random(0, 8);
  }
  switch (f) {
    case 1:
      sendToUART(her, len_srts[0], str0);
      break;
    case 2:
      sendToUART(her, len_srts[1], str1);
      break;
    case 3:
      sendToUART(her, len_srts[2], str2);
      break;
    case 4:
      sendToUART(her, len_srts[3], str3);
      break;
    case 5:
      sendToUART(her, len_srts[4], str4);
      break;
    default:
      RandomMass();
  }
}

void loop() {
  PrintRandomStr(2);
  PrintRandomStr(3);
  PrintRandomStr();
  PrintRandomStr(4);
  PrintRandomStr();
  PrintRandomStr(1);
  delay(1000);
}
