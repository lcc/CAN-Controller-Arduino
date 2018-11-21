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
stack<int> trm_bits;
bool bit_stuff = false;
int bit_stuff_count = 0;
int frame_count = 0;
states state = inter_frame_space;
can_types can_type = schrodinger_frame;
frame my_frame;

int[] bit_mask_CAN_A


inter_frame_space,SOF,arb_phase,control_field,data_field,CRC,ACK,EOFR,overload,error

void bit_mask_setter (){
    if(can_type == can_A){
        bit_mask = [1,12,6,my_frame.dlc*8,15,1,7,1,1,1]
    }
    
    else if(can_type == can_B){
        bit_mask = [1,2,6,64,15]
    }
    
}

void send_frame (){
    
    switch(state){

        case inter_frame_space{
            send_bit(1);
        }
        case arb_phase{

        }
    
    }
}

//This should intitialize all globla variables;
void setting_things_up (){
    bit_stuff_error = false;
    start_seven_recessive_error = false;
    crc_error = false;
    error_bit = false;
    write_bit = false;
    received_bit = false;
    transmited_bit = false;
    last_bit = false;
    stack<int> trm_bits;
    bit_stuff = false;
    bit_stuff_count = 0;
    frame_count = 0;
    state = inter_frame_space;
    can_type = schrodinger_frame;
}

void bit_stuff_error_detect (){
    bool this_bit;
    this_bit = (trm_bits.top());
    
    if(write_bit){
        if( this_bit == last_bit && bit_stuff_count == 5){
            bit_stuff_count = 1;
        }
        else if ( this_bit == last_bit){
            bit_stuff_count += 1;
           // send(this_bit);
            trm_bits.pop();
        }
        else{
            bit_stuff_count = 0;
           //send send(this_bit);
            trm_bits.pop();
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
    this_bit = (trm_bits.top());

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


void get_error(){
    return error_bit;
}

//this function will only be used when writing
void get_next_bit(){
    
}


