#include "data_frame_utils.h"


int main (){
    bool reade_bit = false;
    write_bit = 1;
    setting_things_up();
    //cout << my_frame.id << "\n";
    //cout << '\n' << my_frame.crc << "\n";

    while(state != inter_frame_space){
        reade_bit = set_bit_send();
        //if(state == EOFR){cout  << reade_bit;}
        std::cout.flush();
    }
    //cout << "\n" << my_frame.id << " " << (my_frame.srr | my_frame.rtr) << " " << my_frame.ide << " " << my_frame.id2 << '\n';
    //cout << '\n' << my_frame.dlc << ' ' << my_frame.data << '\n';
    //cout << '\n' << my_frame.r0 << " " << my_frame.r1 << " " << my_frame.dlc << "\n";
    return 0;
}