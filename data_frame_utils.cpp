#include <iostream>
#include "data_frame_utils.h"
#include "math.h"
#include "stdlib.h"
#include "bitset"

using namespace std;


//Initial state of global
bool bit_stuff_erro = false;
bool start_seven_recessive_erro = false;
bool crc_erro = false;
// crc_delimeter,eof,ack_delimiter and intermission (inter_frame_space)
bool form_erro = false;

bool send_ack = true;
bool write_bit = false;

bool last_bit = false;
bool this_bit = false;

bool bit_stuff = false;
int bit_stuff_count = 1;

states state = inter_frame_space;
states erro_state = inter_frame_space;

can_types can_type = schrodinger_frame;
frame my_frame;

int count = 0;
int tail_count = 0;
int bit_pos = 0;
//This should intitialize all globla variables;


void setting_things_up (){
    bit_stuff_erro = false;
    form_erro = false;
    start_seven_recessive_erro = false;
    crc_erro = false;

    this_bit = false;
    last_bit = false;

    bit_stuff = false;
    bit_stuff_count = 0;
    
    state = SOF;
    erro_state = inter_frame_space;

    count = 0;
    tail_count = 0;
    bit_pos = 0;
    
    my_frame_zeros();
    
    
    if(write_bit){
        my_frame_mount();
    }

    can_type = schrodinger_frame;
}


void my_frame_zeros(){
    my_frame.id = 0;
    my_frame.id2 = 0;
    my_frame.dlc = 0;
    my_frame.crc = 0;
    my_frame.data = 0;
    my_frame.sof = 0;
    my_frame.rtr = 0;
    my_frame.srr = 0;
    my_frame.ide = 0;
    my_frame.r0 = 0;
    my_frame.r1 = 0;
    my_frame.crc_delimiter = 1;
    my_frame.ack_slot = 0;
    my_frame.ack_delimeter = 1;
    my_frame.eof = 1;
}
void my_frame_mount(){
    my_frame.id = rand() % (int) pow(2.0,11);
    my_frame.id2 = 0;
    my_frame.dlc = rand() % 9;
    my_frame.data = rand() % (int) (pow(2.0, my_frame.dlc*8 ));
    my_frame.ide = rand() % 2;
    if(my_frame.ide){my_frame.id2 = rand() % (int) pow(2.0,18);}
    my_frame.sof = 0;
    my_frame.rtr = rand() % 2;
    my_frame.srr = 1;

    my_frame.r0 = 0;
    my_frame.r1 = 0;

    my_frame.crc = 0;    
    my_frame.crc_delimiter = 1;

    my_frame.ack_slot = 1;
    my_frame.ack_delimeter = 1;
    my_frame.eof = 1;
}

// falta entrar no estado de erro => em virar um encoder no estado de erro, acho que posso chamar
// setting things_up e depois setar write!
void decoder(bool read_bit){
    
    if(!bit_stuff){mount_package(read_bit);}
    
    if(bit_stuff && read_bit == last_bit){
        erro_state = state;
        state = erro;
    }

    else if(bit_stuff && read_bit != last_bit){bit_stuff = false;bit_stuff_count = 0;}
    
    if(state < ACK){bit_stuff_logic(read_bit);}
    
}

// me dá o bit do tx, ainda não leva em conta arbitração, mas irá.
bool encoder (){
    bool transmiss_bit;

    if(!bit_stuff){transmiss_bit = set_bit_send();}
    else{transmiss_bit = !last_bit; bit_stuff = false;bit_stuff_count=0;}
    if(state < ACK){bit_stuff_logic(transmiss_bit);}        
    
    //lock = false;
    return transmiss_bit;
}


bool set_bit_send (){

        switch(state){
            case SOF:
                count = 1;
                this_bit = my_frame.sof;
                last_bit = this_bit;
                state = arb_phase;
                tail_count = 12;
                break;
            case arb_phase:
                arb_phase_send_logic();
                break;
            case control_field:
                control_field_send_logic();
                break;
            case data_field:
                data_field_send_logic();
                break;
            case CRC:
                crc_field_send_logic();
                break;
            case ACK:
                ack_field_send_logic();
                break;
            case EOFR:
                eof_field_send_logic();
                break;
             //averiguar o número de erro_frame e overload_frame
            case erro:
                erro_frame_logic();
            case overload:
                erro_frame_logic();
            case inter_frame_space:
                inter_frame_space_logic();
        }

    return this_bit;
}

void inter_frame_space_logic(){
    count +=1;
    bit_pos = tail_count - count;
    if(!bit_pos){
        state = idle;
    }
}

void erro_frame_logic(){
    bit_pos = tail_count - count;
    count +=1;
    if (bit_pos > 6){
        this_bit = 0;
    }
    else{
        this_bit = 1;
        if(bit_pos == 1){
            state = inter_frame_space;     
            tail_count +=3;
        }
    }
}


void eof_field_send_logic(){
    bit_pos = tail_count - count;
    count +=1;

    //cout << count << " " << tail_count << " " << bit_pos << "\n";    
    if(bit_pos > 1){
        this_bit = my_frame.eof;
    }

   else{
       this_bit = my_frame.eof;
       state = inter_frame_space;
    }
}

void ack_field_send_logic(){
    bit_pos = tail_count - count ;
    count+=1;
   // cout << count << " " << tail_count << " " << bit_pos << " ";
    
    if(bit_pos == 2){
        this_bit = my_frame.ack_slot;
        //cout << this_bit << "\n";
    
    }    
    else if(bit_pos == 1){
        this_bit = my_frame.ack_delimeter;
        state = EOFR;
        tail_count = count + 7;
        //cout << this_bit << "\n";
    }
}

void crc_field_send_logic(){
    bit_pos = tail_count - count;
    count+=1;
    if(count <= tail_count){
        //cout << count << " " << tail_count << " " << bit_pos << " ";
        this_bit = (my_frame.crc >> (bit_pos - 1)) & 1;
        //cout << this_bit << "\n";
        if(bit_pos == 1){
            bit_stuff_count  = 0;
            this_bit = my_frame.crc_delimiter;
            state = ACK;
            tail_count = count + 2;
        }
    }
}

void data_field_send_logic(){
    bit_pos = tail_count - count;
    count+=1;
    if(!my_frame.rtr){
        if(count <= tail_count){
            this_bit = (my_frame.data >> (bit_pos - 1)) & 1;
            if(bit_pos == 1){
                state = CRC;
                tail_count = tail_count + 16;
            }
        }
        
        else{state = CRC;tail_count = tail_count + 16;}
    }
    
    else{count-=1;state = CRC;tail_count = count + 16;crc_field_send_logic();}

}

void control_field_send_logic(){
    bit_pos = tail_count - count;
    count += 1;

    if(can_type == can_A){
        //cout << count << " " << tail_count << " " << bit_pos << "\n";
        if (bit_pos == 5){
            this_bit = my_frame.r0;
        }
        else if( bit_pos < 5){
            this_bit = (my_frame.dlc >> (bit_pos - 1)) & 1;
             if(bit_pos == 1){
                state = data_field;
                tail_count = tail_count + my_frame.dlc * 8;
            }
        }

    }
    if(can_type == can_B){
        //cout << count << " " << tail_count << " " << bit_pos << "\n";
        if (bit_pos == 7){
            this_bit = my_frame.rtr;
        }
        else if(bit_pos == 6){
            this_bit = my_frame.r1;
        }
        else if(bit_pos == 5){
            this_bit = my_frame.r0;
        }
        else if(bit_pos < 5){
            this_bit = (my_frame.dlc >> (bit_pos -1 )) & 1;
            if(bit_pos == 1){
                state = data_field;
                tail_count = count + my_frame.dlc * 8;
            }
        }
    }
}

void arb_phase_send_logic () {
    bit_pos = tail_count - count;
    count += 1;
    if(count == 14){
        if(!my_frame.ide){
            can_type = can_A;
            tail_count = 19;
            state = control_field;
        }
        else{
            can_type = can_B;
            tail_count = 32;
            bit_pos = tail_count - count;     
        }
    }
    if(count <= tail_count+1){
        if(count <= 12){
            this_bit  = (my_frame.id >> bit_pos - 1) & 1;
        }
        else if(count == 13){
            if(my_frame.ide){
                this_bit =my_frame.srr;
            }
            else{
                this_bit = my_frame.rtr;
            }
        }

        else if(count == 14){
            this_bit = my_frame.ide;
        }
        else if (count < 32){
            this_bit = (my_frame.id2 >> bit_pos  - 1) & 1;
        }
        else if(count == 32){
                this_bit = (my_frame.id2 >> bit_pos - 1) & 1;
                state = control_field;
                tail_count +=7;
            }

    }

}

void mount_package(bool read_bit) {

    switch(state){
        case SOF:
            count = 1;
            my_frame.sof = 0;
            state = arb_phase;
            tail_count = 12;
            break;
        case arb_phase:
            arb_field_mount(read_bit);
            break;
        case control_field:
            control_field_mount(read_bit);
            break;
        case data_field:
            data_field_mount(read_bit);
            break;
        case CRC:
            crc_field_mount(read_bit);
            break;
        case ACK:
            ack_field_mount(read_bit);
            break;
        case EOFR:
            eof_field_mount(read_bit);
        case inter_frame_space:
            inter_frame_space_check(read_bit);
            break;
    }
    
    if(state == erro || state == overload){
        write_bit = 1;
        // erro frame vai durar mais uns 12 bits
        tail_count = count + 12;
    }
    
}

void inter_frame_space_check(bool read_bit){
    count+=1;
    
    if(count < tail_count){
        if(!read_bit){erro_state = state;state = overload; form_erro = true;}
    }

    else{
        if(!read_bit){erro_state = state; state = erro; form_erro = true;}
        else{state = idle;}
    }
}

void eof_field_mount (bool read_bit){
    count += 1;
    if(count < tail_count){
        if(!read_bit){
            erro_state = state;
            state = erro;
            form_erro = true;
        }
    }
    else{
        my_frame.eof = read_bit;    
        state = inter_frame_space;
        tail_count += 3;
    }
}


void ack_field_mount(bool read_bit){
    count+=1;
    if(count < tail_count){
        send_ack = 0;
    }    
    else{
        my_frame.ack_delimeter = read_bit;
        state = EOFR;
        if(!my_frame.ack_delimeter){erro_state=state;state=erro;form_erro = true;}
        tail_count = tail_count + 7;
    }
}

void crc_field_mount(bool read_bit){
    count+=1;
    if(count < tail_count){
        my_frame.crc = (my_frame.crc << 1) | read_bit;
    }
    else {
        bit_stuff_count = 0;
        my_frame.crc_delimiter = read_bit;
        state = ACK;
        if(!my_frame.crc_delimiter){form_erro = true;state = erro;}
        tail_count = count + 2;
    }
}

void data_field_mount(bool read_bit){
    count+=1;
    if(!my_frame.rtr && my_frame.dlc != 0){
        if(count < tail_count){
            //cout << count << " " << tail_count << " " << bit_pos << " ";
            my_frame.data = (my_frame.data << 1) | read_bit;
            //cout << this_bit << "\n";
        }

        else{
            my_frame.data = (my_frame.data << 1) | read_bit;
            state = CRC;
            tail_count = tail_count + 16;
        }
    }
    else{state = CRC; tail_count = count + 16 ;crc_field_mount(read_bit);}
    
}

void control_field_mount (bool read_bit){
    bit_pos = tail_count - count;
    count += 1;

   if(can_type == can_A){
        //cout << '\n' << count << " " << tail_count << " " << bit_pos << "\n";
        if (bit_pos == 5){
            my_frame.r0 = read_bit;
        }
        else if( bit_pos < 5){
             my_frame.dlc  =  (my_frame.dlc << 1) | read_bit;
             if(bit_pos == 1){
                state = data_field;
                dlc_correction();
                tail_count = tail_count + my_frame.dlc * 8;
            }
        }
    }

    else if(can_type == can_B){
        //cout << '\n' << count << " " << tail_count << " " << bit_pos << "\n";
        if (bit_pos == 7){
            cout << "-----" << read_bit << "------" << count;
             my_frame.rtr = read_bit;
        }
        else if(bit_pos == 6){
             my_frame.r1 = read_bit;
        }
        else if(bit_pos == 5){
             my_frame.r0 = read_bit;
        }
        else if(bit_pos < 5){
            my_frame.dlc =  (my_frame.dlc << 1) | read_bit;
            if(bit_pos == 1){
                state = data_field;
                dlc_correction();
                tail_count = count + my_frame.dlc * 8;
            }
        }
    }

}

void arb_field_mount(bool read_bit){
    count +=1;    
     
     if(count == 14){
        if(!read_bit){
            can_type = can_A;
            my_frame.rtr = last_bit;
            tail_count = 19;
            state = control_field;
        }
        else{
            can_type = can_B;
            my_frame.srr = last_bit;
            tail_count = 32;
        }
    }

    if(count <= tail_count){
        if(count <= 12){
            my_frame.id  = (my_frame.id << 1) | read_bit;
        }
        else if(count == 13){
            last_bit = read_bit;
        }
        else if(count == 14){
            my_frame.ide = read_bit;
        }
        else if (count < 32){
            my_frame.id2 = (my_frame.id2 << 1) | read_bit;
        }
        else if (count == 32){
            my_frame.id2 = (my_frame.id2 << 1) | read_bit;
            state = control_field;
            tail_count += 7;         
        }
    }

}

 
void bit_stuff_logic(bool read_bit){
    if(write_bit){
        if(last_bit == read_bit){
            bit_stuff_count += 1;
            // vou ter que escrever o próximo bit como !last
            if(bit_stuff_count == 5){
                bit_stuff = true;
            }
        }
        //segue jogo
        else{
            bit_stuff_count = 1;
            last_bit = read_bit;
        }
    }

    else{
         if(read_bit == last_bit){
            bit_stuff_count += 1;
            //próximo bit tem que ser de stuff
           // cout << " " <<  count << " ";
            if(bit_stuff_count == 5){
                bit_stuff = true;
            }
        }
        else{
            last_bit = read_bit;
            bit_stuff_count = 1;
        }
    }
    
}

void state_name (states curr_state){
    switch(curr_state){    
        case SOF:
            cout << "SOF" << "\n";
            break;
        case arb_phase:
            cout << "arb_phase" << '\n';
            break;
        case control_field:
            cout << "control_field" << '\n';
            break;
        case data_field:
            cout << "data_field"  << '\n';
            break;
        case CRC:
            cout << "CRC" << '\n';
            break;
        case ACK:
            cout << "ACK" << '\n';
            break;
        case EOFR:
            cout << "end_of_frame" << '\n';
        case inter_frame_space:
            cout << "inter_frame_space" << '\n';
            break;
        case idle:
            cout << "idle" << '\n';
            break;
        case erro:
            cout << "erro" << '\n';
            break;
        case overload:
            cout << "overload" << '\n';
            break;
    }
}


// if lost arbitration,call this
void change_mode (bool read_bit){
    int aux = 0, aux2 = 0;
    bool aux_bool,aux_bool2,aux_bool3 ;
    write_bit = false;
    
    if(count <= 12){
        aux = my_frame.id = (my_frame.id >>  (tail_count - count)) << 1;
        my_frame_zeros();
        my_frame.id = aux;
    }
    // pode ter problemas com o srr e rtr? não o read_Bit seta um valor novo, nao checa se tem uma antigo
    else if(count == 13){
        aux = my_frame.id;
        aux_bool = my_frame.rtr;
        my_frame_zeros();            
        my_frame.id = aux;
        my_frame.rtr = aux_bool;
        my_frame.srr = aux_bool;
    }
    else if(count == 14){
        aux = my_frame.id;
        aux_bool = my_frame.rtr;
        aux_bool2 = my_frame.ide;
        my_frame_zeros();            
        my_frame.id = aux;
        my_frame.srr = aux_bool;
        my_frame.rtr = aux_bool;
        my_frame.ide = aux_bool2;
    }

    else if (count > 14 && count <= 32){
        aux = my_frame.id;
        aux_bool = my_frame.srr;
        aux_bool2 = my_frame.ide;
        aux2 = my_frame.id2 = (my_frame.id2 >>  (tail_count - count)) << 1;
        my_frame_zeros();            
        my_frame.id = aux;
        my_frame.rtr = aux_bool;
        my_frame.srr = aux_bool;
        my_frame.ide = aux_bool2;
        my_frame.id2 = aux2;
    }
    else{
        aux = my_frame.id;
        aux_bool = my_frame.srr;
        aux_bool2 = my_frame.ide;
        aux2 = my_frame.id2 = (my_frame.id2 >>  (tail_count - count)) << 1;
        aux_bool3 = my_frame.rtr;
        my_frame_zeros();            
        my_frame.id = aux;
        my_frame.rtr = aux_bool3;
        my_frame.srr = aux_bool;
        my_frame.ide = aux_bool2;
        my_frame.id2 = aux2;
    }
}


void dlc_correction(){
    if(my_frame.dlc > 8){
        my_frame.dlc = 8;
    }
}


