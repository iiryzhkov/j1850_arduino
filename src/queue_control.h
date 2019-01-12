#ifndef QUEUE
#define QUEUE

#include <Arduino.h>

#define DIVIDER -2

class queue_control{
    protected:
        int *queue;
        int current_size = 0;
        int max_size = 0;
        bool auto_remove=false;

    public:
        queue_control(int _size, bool _auto_remove=false){
            queue = new int[_size];
            max_size = _size;
            auto_remove = _auto_remove;
        }
        bool add_event(int event){
            if (current_size == max_size) {
                if(auto_remove){
                    count_event();
                }else{
                    return false;
                }
            }
            queue[current_size++] = event;
            return true;
        }

        int count_event(){
            if(!current_size) return -1;
            int ret = queue[0];
            current_size--;
            for(int i=0; i < current_size; i++) queue[i] = queue[i + 1];
            queue[current_size]=0;
            return ret;
        }

        int get_size(){
            return current_size;
        }

        void remove_queue(){
            current_size = 0;
            for(int i = 0; i < max_size; i++) queue[i] = 0;
        }

        int last_item(){
            if(!current_size) return -1;
            return queue[current_size - 1];
        }

        int first_item(){
            if(!current_size) return -1;
            return queue[0];
        }
};

class queue_control_array : public queue_control{
    public:
        queue_control_array(int _size) : queue_control(_size) {}
        int get_divider()
        {
            return DIVIDER;
        }
        bool check_size(int len){
            return (get_size() < max_size - len);
        }

        bool add_array(byte * buffer, int len)
        {                        
            if (check_size(len)){
                for(int i=0;i<len;i++){
                    add_event(buffer[i]);
                }
                add_event(get_divider());
                return true;
            }
            return false;
        }

        int count_array(byte * buffer, int max_len, int bias=0){
            int tmp_byte = 0; 
            int i = 0;
            if (last_item() != get_divider()){
                return -1;
            }
            while(i <= max_len){
                tmp_byte = count_event();
                if (tmp_byte < 0){
                    break;
                }
                buffer[i + bias] = tmp_byte;
                i++;
            }
            if (i <= max_len && tmp_byte == get_divider()){
                return i;
            }else{
                return -1;
            }
        }
};
#endif