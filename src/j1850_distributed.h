#ifndef J1850_D
#define J1850_D
#include "j1850.h"
#include "queue_control.h"

#define J1850_MESSAGE_FILTERED 10
#define WIRE_WRITE_BUFFER_IS_NOT_EMPTY 11
#define WIRE_READ_BUFFER_IS_NOT_EMPTY 12
#define WIRE_WRITE_OK 13
#define WIRE_READ_OK 14

#define TIMEOUT_ACCEPT_DATA_US 200000
#define SPEAD_I2C 3400000
#define ADDRESS_I2C 8
#define LEN_BUFFER_R 255
#define LEN_BUFFER_W 255

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