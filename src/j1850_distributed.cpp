#include "j1850_distributed.h"
#include <Wire.h>
#include <stdarg.h>


j1850_slave *j1850_slave::instances = NULL;

void j1850_slave::init(int in_pin_, int out_pin_, Print *pr_, int address, long speed)
{
    TestSlave::init(in_pin_, out_pin_, pr_);
    Wire.begin(address);
    Wire.setClock(speed);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);
    instances = this;
}

void j1850_slave::set_monitoring(int mode_)
{
	if((mode_ > 6) or (mode_ < 0)){
		pr->println("The mode should be from 0 to 6");
		return;
	}
	monitoring_mode = mode_;
}

void j1850_slave::len_bufer()
{
    static int old_read_len = 0;
    static int old_write_len = 0;
    int read_len = read_buf.get_size();
    int write_len = write_buf.get_size();
    if(old_read_len != read_len){
        pr->print("R: ");
        pr->println(read_len);
        old_read_len = read_len;
    }
    if(old_write_len != write_len){
        pr->print("W: ");
        pr->println(write_len);
        old_write_len = write_len;
    }
}

void j1850_slave::__receiveEvent(int howMany)
{
    if (!write_buf.check_size(howMany)){
        message = WIRE_WRITE_BUFFER_IS_NOT_EMPTY;
        return;
    }
    while (0 < Wire.available())
    {
        write_buf.add_event(Wire.read());
    }
    write_buf.add_event(write_buf.get_devider());
    message = WIRE_READ_OK;
}

void j1850_slave::__requestEvent()
{
    byte tmp_read[13] = {0};
    int len = read_buf.count_array(tmp_read, 12, 1);
    
    if(len > 0){
        tmp_read[0] = len;
        Wire.write(tmp_read, len + 1);
        message = WIRE_WRITE_OK;
    }else{
        Wire.write(0);
    }
}

void j1850_slave::loop()
{
    byte tmp_buf[12] = {0};
    if (accept(tmp_buf, true))
    {
        if (filter(tmp_buf, rx_nbyte))
        {
            if(!read_buf.add_array(tmp_buf, rx_nbyte)){
                message = WIRE_READ_BUFFER_IS_NOT_EMPTY;
            }
        }else{
            message = J1850_MESSAGE_FILTERED;
        }
    }
    int len = write_buf.count_array(tmp_buf, 12);
    if(len > 0){
        send(tmp_buf, len);
    }
    if (monitoring_mode == 6){
        len_bufer();
    }
}

void j1850_slave::set_filter(func f)
{
    filter = f;
}

void j1850_master::init(int _address, long speed, Print *pr_)
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
    rx_nbyte = 0;
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
        rx_nbyte = len;
    }
    if (monitoring_mode > 0){
        monitor();
    }
    return (bool)len;
}

bool j1850_master::send(byte *msg_buf, int nbytes)
{
    message = 0;
    tx_nbyte = 0;
    tx_msg_buf = msg_buf;
    bool res = false;
    Wire.beginTransmission(address);
    Wire.write(msg_buf, nbytes);
    if ((Wire.endTransmission() == 0) && (nbytes > 0))
    {
        res = true;
        message = MESSAGE_SEND_OK;
        tx_nbyte = nbytes;
    }
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