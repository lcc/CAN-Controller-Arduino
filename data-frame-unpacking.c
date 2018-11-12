#include <stdio.h>

typedef enum { false, true } bool;
typedef enum {inter_frame_space,arb_phase,control_field,data_field,CRC,ACK,EOFR,overload,error} states;
typedef enum {schrodinger_frame,can_A,can_B} can_types;

// Standard values for the CAN protocol
// can_A_arb_length =  ID + RTR = 1 + 11 
#define can_A_arb_length = 12;
// can_A_arb_length = ID + SRR + IDE + ID_2 + RTR = 11 + 1 + 1 + 18 + 1 =
#define can_B_arb_length = 32;
#define control_field_length = 6;

// Flags responsible for triggering errors
bool bit_stuff_error = false;
bool start_seven_recessive_error = false;
bool crc_error = false;

//Bits responsible for logic control of reading and writing 
bool write_bit = 0;
bool received_bit = 0;
bool transmited_bit = 0;
bool last_bit = 0;
bool this_bit = 0;
bool next_bit = 0;

//Auxiliar vars, mostly counters
int bit_stuff_count = 0;
int frame_count = 0;

//Var responsible for keeping track of the state of the system
states state = inter_frame_space;
can_types can_type = schrodinger_frame;



int main () {

    while(1){

        switch (state) {
            
            case inter_frame_space:

            break;

            case arb_phase:

            break;

            case control_field:

            break;

            case data_field:

            break;

            case CRC:
            
            break;

            case ACK:

            break;

            case EOFR:

            break;

            case overload:

            break;

            case error:

            break;
        }
    }
    return 0;
}

void bit_stuff_error_detect (){
// talvez implementar com um pilha as funções do this_bit e do next+
    if(write_bit){
        if(this_bit == last_bit && bit_stuff_count == 6){
            bit_stuff_count = 1;
            send(!last_bit);
        }
        else if (this_bit == last_bit){
            bit_stuff_count += 1;
            send(this_bit);
        }
        else{
            bit_stuff_count = 0;
            send(this_bit);
        }

    }
    else {
        if(this_bit == last_bit){
            bit_stuff_count += 1;             
       }
       else{
           bit_stuff_count = 0;
       }

        if(bit_stuff_count == 6){
            bit_stuff_error == true;
        }
    }
}


void arb_phase_method (){
}