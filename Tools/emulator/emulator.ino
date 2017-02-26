
int len_srts[] = {4, 5, 2, 3, 6};

byte str0[] = { 0x01, 0x02, 0x02, 0x03};
byte str1[] = { 0xE1, 0xB2, 0x35, 0xFF, 0x02};
byte str2[] = { 0xE1, 0xB3};
byte str3[] = { 0xA1, 0xF2, 0x35};
byte str4[] = { 0x01, 0x02, 0x02, 0x03, 0xFF, 0x02};



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void sendToUART(const char* header, int rx_nbyte, byte *msg_buf) {
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
  int i = random(2, 6);
  int r = i;
  for (i >= 0; i--;) {
    tmp[i] = random(0, 254);
  }
  sendToUART("RX: ", i, tmp);
}

void PrintRandomStr(int f = 0) {
  int jopa = 0;
  if (f == 0) {
    f = random(0, 8);
  }
  switch (f) {
    case 1:
      sendToUART("RX: ", len_srts[0], str0);
      break;
    case 2:
      sendToUART("RX: ", len_srts[1], str1);
      break;
    case 3:
      sendToUART("RX: ", len_srts[2], str2);
      break;
    case 4:
      sendToUART("RX: ", len_srts[3], str3);
      break;
    case 5:
      sendToUART("RX: ", len_srts[4], str4);
      break;
    case 6:
      RandomMass();
    default:
      break;
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
