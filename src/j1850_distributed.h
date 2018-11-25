#ifndef J1850_D
#define J1850_D
#include "j1850.h"
#include "queue_control.h"

#define J1850_MESSAGE_FILTERED 10
#define WIRE_WRITE_BUFFER_IS_NOT_EMPTY 11
#define WIRE_READ_BUFFER_IS_NOT_EMPTY 12
#define WIRE_WRITE_OK 13
#define WIRE_READ_OK 14

#define TIMEOUT_ACCEPT_DATA_US 4000
#define SPEAD_I2C 3400000
#define ADDRESS_I2C 8
#define LEN_BUFFER_R 255
#define LEN_BUFFER_W 255


class TestSlave
{
  public:
    byte x[12];
    int rx_nbyte = 0;
    bool if_read = false;
    int message = 0;
    Print* pr;
    int monitoring_mode = 0;
    void init(int, int, Print* pr_ = &Serial)
    {
        pr = pr_;
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
    void init(int, int, Print *pr_ = &Serial, int address = ADDRESS_I2C, long speed = SPEAD_I2C);
    void __receiveEvent(int);
    void __requestEvent();
    void loop();
    void set_filter(func);
    void set_monitoring(int mod = 1);

  private:
    func filter;
    void len_bufer();
    queue_control_array read_buf = queue_control_array(LEN_BUFFER_R);
    queue_control_array write_buf = queue_control_array(LEN_BUFFER_W);
};

class j1850_master : public j1850
{
  public:
    void init(int _address = ADDRESS_I2C, long speed = SPEAD_I2C, Print *pr_ = &Serial);
    bool accept(byte *, bool);
    bool send(byte *, int);
    bool easy_send(int, ...);
  private:
    int address = 0;
};
#endif