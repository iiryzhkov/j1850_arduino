#include "j1850_distributed.h"
#include <Wire.h>
#include <stdarg.h>


j1850_slave *j1850_slave::instances = NULL;

void j1850_slave::init(int in_pin_, int out_pin_, Print *pr_, int address, int speed)
{
    j1850::init(in_pin_, out_pin_, pr_);
    Wire.begin(address);
    Wire.setClock(speed);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    instances = this;
}

void j1850_slave::__receiveEvent(int howMany)
{
    int i = 0;
    if (write_bytes > 0)
    {
        while (0 < Wire.available())
        {
            Wire.read();
        }
        message = WIRE_WRITE_BUFFER_IS_NOT_EMPTY;
    }
    else
    {
        while (0 < Wire.available())
        {
            buff_write[i++] = Wire.read();
        }
        message = WIRE_READ_OK;
        write_bytes = i;
    }
}

void j1850_slave::__requestEvent()
{
    if (read_bytes > 0)
    {
        Wire.write(buff_read, read_bytes);
        read_bytes = 0;
        message = WIRE_WRITE_OK;
    }
    else
    {
        Wire.write(0);
    }
}

void j1850_slave::loop()
{
    byte tmp_read_buff[12];
    if (accept(tmp_read_buff, true))
    {
        if (!filter(tmp_read_buff, rx_nbyte))
        {
            message = J1850_MESSAGE_FILTERED;
            return;
        }
        if (read_bytes == 0)
        {
            buff_read[0] = rx_nbyte;
            for (int i = 0; i < rx_nbyte; i++)
            {
                buff_read[i + 1] = tmp_read_buff[i];
            }
            read_bytes = rx_nbyte + 1;
        }
        else
        {
            message = WIRE_READ_BUFFER_IS_NOT_EMPTY;
        }
    }
    if (write_bytes > 0)
    {
        send(buff_write, write_bytes);
        write_bytes = 0;
    }
}

void j1850_slave::set_filter(func f)
{
    filter = f;
}

void j1850_master::init(int _address, int speed, Print *pr_)
{
    address = _address;
    pr = pr_;
    Wire.begin();
    Wire.setClock(speed);
}

bool j1850_master::accept(byte *msg_buf, bool timeout)
{
    rx_msg_buf = msg_buf;
    message = 0;
    if (timeout)
    {
        if (read_timer() < TIMEOUT_ACCEPT_DATA_US){
            return false;
        }
        start_timer();
    }
    Wire.requestFrom(address, 13);
    int len = Wire.read();
    for (int i = 0; i < len; i++)
    {
        msg_buf[i] = Wire.read();
    }
    if (len > 0)
    {
        message = MESSAGE_ACCEPT_OK;
    }
    rx_nbyte = len;
    if (monitoring_mode > 0){
        monitor();
    }
    return (bool)len;
}

bool j1850_master::send(byte *msg_buf, int nbytes)
{
    message = 0;
    tx_msg_buf = msg_buf;
    bool res = false;
    Wire.beginTransmission(address);
    Wire.write(msg_buf, nbytes);
    if ((Wire.endTransmission() == 0) && (nbytes > 0))
    {
        res = true;
        message = MESSAGE_SEND_OK;
    }
    tx_nbyte = nbytes;
    if (monitoring_mode > 0)
    {
        monitor();
    }
    return res;
}

bool j1850_master::easy_send(int size, ...)
{
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