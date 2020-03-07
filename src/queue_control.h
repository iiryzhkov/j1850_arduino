#ifndef QUEUE
#define QUEUE

#include <Arduino.h>

#define DIVIDER -2

class queue_control{
    protected:
        int *queue;
        int cursor = 0;
        int last_number = 0;
        int max_size = 0;
        bool auto_remove = false;
        bool empty = true;

        int next_number(int *val){
            (*val)++;
            if (*val == max_size) (*val) = 0;
        }

    public:
        queue_control(int _size, bool _auto_remove=false){
            queue = new int[_size];
            max_size = _size;
            auto_remove = _auto_remove;
        }
        bool add_event(int event){
            if (empty){
                empty = false;
                queue[last_number] = event;
                return true;
            } else if (get_size() == max_size) {
                if (auto_remove){
                    next_number(&cursor);
                } else {
                    return false;
                }
            }
            next_number(&last_number);
            queue[last_number] = event;
            return true;
        }

        int count_event(){
            if(empty) return -1;
            int res = queue[cursor];
            if (cursor == last_number){
                empty = true;
            } else {
                next_number(&cursor);
            }
            return res;
        }

        int get_size(){
            if (empty) {
                return 0;
            } else if (cursor == last_number) {
                return 1;
            } else if (last_number > cursor) {
                return last_number + 1 - cursor;
            }
            return max_size - cursor + last_number + 1;
        }

        void remove_queue(){
            cursor = 0;
            last_number = 0;
            empty = true;
        }

        int last_item(){
            if(empty) return -1;
            return queue[last_number];
        }

        int first_item(){
            if(empty) return -1;
            return queue[cursor];
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