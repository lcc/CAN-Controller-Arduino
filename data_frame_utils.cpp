#include "data_frame_utils.h"

//Initial state of global
bool bit_stuff_error = false;
bool start_seven_recessive_error = false;
bool crc_error = false;
bool error_bit = false;

bool write_bit = false;

bool received_bit = false;
bool transmited_bit = false;
bool last_bit = false;
bool this_bit = false;
// this_bit will probably substitute transmitted bit

bool bit_stuff = false;
int bit_stuff_count = 0;
int frame_count = 0;
//frame_count is the count the same as the count var I guess

states state = inter_frame_space;
can_types can_type = schrodinger_frame;
frame my_frame;

int count = 0;
int tail_count = 0;
int bit_pos = 0;
// this one is needed bcuz tail count will be used to control
// the end of arb_phase

//This should intitialize all globla variables;
void setting_things_up (){
    bit_stuff_error = false;
    start_seven_recessive_error = false;
    crc_error = false;
    error_bit = false;

    received_bit = false;
    this_bit = false;
    
    transmited_bit = false;
    last_bit = false;
    bit_stuff = false;
    bit_stuff_count = 0;
    frame_count = 0;
    
    state = SOF;

    count = 0;
    tail_count = 0;
    bit_pos = 0;
    
    if(write_bit){
        my_frame_mount();
    }

    else {
        my_frame_zeros();
    }
    can_type = schrodinger_frame;
}


void my_frame_zeros(){
    my_frame.id = 0;
    my_frame.id2 = 0;
    my_frame.dlc = 0;
    my_frame.crc = 0;
    my_frame.data = 0;
    my_frame.sof = 1;
    my_frame.rtr = 0;
    my_frame.srr = 0;
    my_frame.ide = 0;
    my_frame.r0 = 0;
    my_frame.r1 = 0;
    my_frame.crc_delimiter = 0;
    my_frame.ack_slot = 0;
    my_frame.ack_delimeter = 0;
    my_frame.eof = 0;
}
void my_frame_mount(){
    my_frame.id = rand() % (int) pow(2,11);
    my_frame.id2 = 0;
    my_frame.dlc = rand() % 9;
    my_frame.data = rand() % (int) (pow(2, my_frame.dlc*8 ));
    my_frame.ide = rand() % 2;
    if(my_frame.ide){my_frame.id2 = rand() % (int) pow(2,18);}
    my_frame.sof = 1;
    my_frame.rtr = rand() % 2;
    my_frame.srr = 1;

    my_frame.r0 = 0;
    my_frame.r1 = 0;

    my_frame.crc = 0;    
    my_frame.crc_delimiter = 1;

    my_frame.ack_slot = 1;
    my_frame.ack_delimeter = 0;
    my_frame.eof = 1;
}

bool set_bit_send (){
    
    switch(state){
        case SOF:
            count = 1;
            this_bit = my_frame.sof;
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
    }
    return this_bit;
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
        this_bit = (my_frame.crc >> bit_pos - 1) & 1;
        //cout << this_bit << "\n";
        if(bit_pos == 1){
            this_bit = my_frame.crc_delimiter;
            state = ACK;
            tail_count = count + 2;
        }
    }
}

void data_field_send_logic(){
    bit_pos = tail_count - count;
    count+=1;
    if(count <= tail_count){
        //cout << count << " " << tail_count << " " << bit_pos << " ";
        this_bit = (my_frame.data >> (bit_pos - 1)) & 1;
        //cout << this_bit << "\n";
        if(bit_pos == 1){
            state = CRC;
            tail_count = tail_count + 16;
        }
    }

}

void control_field_send_logic(){
    bit_pos = tail_count - count;
    count += 1;

    if(can_type == can_A){
        if (bit_pos == 5){
            this_bit = my_frame.r0;
        }
        else if(bit_pos == 0){
            state = data_field;
            tail_count = tail_count + my_frame.dlc * 8;
        }
        else if( bit_pos < 5){
            this_bit = (my_frame.dlc >> (bit_pos - 1)) & 1;
        }

    }
    if(can_type == can_B){
        //cout << count << " " << tail_count << " " << bit_pos << " ";
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
    //cout << this_bit << "\n";
}
// cout inside arb_pahse: 11010110011111 001101110001010001
//                        11010110011111 001101110001010001
//cout inside case-switch:11010110011111 001101110001010001
//for real cout          :11010110011111 001101110001010001
// okidoki;
void arb_phase_send_logic ()    {
    bit_pos = tail_count - count;
    // sets things up if the bit is recessive     
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
    // can_A => arb_count = 14 (assim que recebe o RTR) e can_B => arb_count = 32 (recebe o ID)
    if(count <= tail_count){
        // my frame id = 11110011010; my_frame_id >> (12 - 2) -> 10; 
        if(count <= 12){
            this_bit  = (my_frame.id >> bit_pos - 1) & 1;
        }
        // doesn't matter what type of frame it's, it'll get the right bit
        // this takes into account that when a frame is made, no mistakes are
        // commited
        // all bits must be set to false b4.
        else if(count == 13){
            this_bit = my_frame.rtr | my_frame.srr;
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

void bit_stuff_error_detect (){
    bool this_bit;
    
    if(write_bit){
        if( this_bit == last_bit && bit_stuff_count == 5){
            bit_stuff_count = 1;
        }
        else if ( this_bit == last_bit){
            bit_stuff_count += 1;
           // send(this_bit);
        }
        else{
            bit_stuff_count = 0;
           //send send(this_bit);
        }
    }
    else {
        if( bit_stuff_count == 5 && this_bit != last_bit){
            bit_stuff = true;
            bit_stuff_count = 0;
        }
        else if(this_bit == last_bit){
            bit_stuff_count += 1;             
       }
       else{
           bit_stuff_count = 0;
       }
        if(bit_stuff_count == 6){
            bit_stuff_error = true;
        }
    }
}

// monta os pacotes para os nós que estão lendo/escrevendo(no caso de uma perda de barramento)?
// talvez haja um jeito melhor, guardar o bit caso perca e ir montando o frame daí!
void packet_mount (bool this_bit){
    // faria um switch case com todos os possiveis estados e montaria o pacote de acordo com o 
    // estado e o count!
    // desconsidera bits de bit stuffing
    if(!bit_stuff){
        if(state == arb_phase){
            if( (frame_count <= 11) ||  (can_type == can_B && frame_count > 13) )
                my_frame.id = (my_frame.id << 1) || this_bit;
            // quando frame_count é 12, nada é feito
            if(frame_count == 13 && can_type == can_A){
                my_frame.rtr = last_bit;
                my_frame.ide = this_bit;
            }
            else if(frame_count == 13 && can_type == can_B){
                my_frame.srr = last_bit;
                my_frame.ide = this_bit;
            }
        }
        
    }
    // does nothing for stuffed bit
    else{
        bit_stuff = false;
    }
}

// Isso faz sentido quando estiver enviando o pacote? Não, para enviar basta montar o pacote e enviar no tempo certo.
// Isso sempre checando o bit_stuff claro.
// esse é o metódo apenas quando estou lendo e não quando estou escrevendo, porquê? montagem do pacote
void arb_phase_method (){
    bool this_bit;

    if( (frame_count == 13 && can_type == can_A) || (frame_count == 33 && can_type == can_B))
        state = control_field;
    else{
        //Se o contador está em 12 é porque estou recebendo o bit 13
        if(frame_count == 12)
            if(this_bit == true){can_type = can_A;}
            else{can_type = can_B;}
    }
    packet_mount(this_bit);
    frame_count += 1;
}


bool get_error(){
    return error_bit;
}