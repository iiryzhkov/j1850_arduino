#ifndef J1850_D
#define J1850_D
#include "j1850.h"

#define J1850_MESSAGE_FILTERED 9
#define WIRE_WRITE_BUFFER_IS_NOT_EMPTY 10
#define WIRE_READ_BUFFER_IS_NOT_EMPTY 11
#define WIRE_WRITE_OK 12
#define WIRE_READ_OK 13

#define TIMEOUT_ACCEPT_DATA_US 1000

class TestSlave
{
  public:
    byte x[12];
    int rx_nbyte = 0;
    bool if_read = false;
    int message = 0;
    void init(int a, int s, Print *)
    {
        return;
    }
    void set_monitoring(int x)
    {
        return;
    }
    bool accept(byte *buff, bool a)
    {
        if (if_read)
        {
            rx_nbyte = 0;
        }
        if (rx_nbyte > 0)
        {
            for (int i = 0; i < rx_nbyte; i++)
            {
                buff[i] = x[i];
            }
            // sendToUART("RX: ", rx_nbyte, buff);
            if_read = true;
            return true;
        }
        return false;
    }
    void send(byte *buuf, int f)
    {
        if (f > 0)
        {
            for (int i = 0; i < f; i++)
            {
                x[i] = buuf[i];
            }
            // sendToUART("TX: ", f, x);
            if_read = false;
            rx_nbyte = f;
        }
    }
    void sendToUART(const char *header, uint8_t nbyte, byte *msg_buf)
    {
        Serial.print(header);
        for (uint8_t i = 0; i < nbyte; i++)
        {
            if (msg_buf[i] < 0x10)
                Serial.print(0);

            Serial.print(msg_buf[i], HEX);
            Serial.print(" ");
        }
        Serial.print("\n");
    }
};

class j1850_slave : public j1850
{
    typedef bool (*func)(byte* a, int b);
    static j1850_slave *instances;
    static void receiveEvent(int howMany)
    {
        j1850_slave::instances->__receiveEvent(howMany);
    }
    static void requestEvent()
    {
        j1850_slave::instances->__requestEvent();
    }

  public:
    void init(int, int, Print *pr_ = &Serial, int address = 8, int speed = 3400000);
    void __receiveEvent(int);
    void __requestEvent();
    void loop();
    void set_filter(func);

  private:
    func filter;
    byte buff_read[13];
    volatile byte buff_write[12];
    volatile int read_bytes = 0;
    volatile int write_bytes = 0;
};

class j1850_master : public j1850
{
  public:
    void init(int _address = 8, int speed = 3400000, Print *pr_ = &Serial);
    bool accept(byte *, bool);
    bool send(byte *, int);
    bool j1850_master::easy_send(int, ...);
  private:
    int address = 0;
};
#endif