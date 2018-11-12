#include <stdio.h>

typedef enum { false, true } bool;
typedef enum {inter_frame_space,arb_phase,control_field,data_field,CRC,ACK,EOFR,overload,error} states;
typedef enum {schrodinger_frame,can_A,can_B} can_types;

//TODO: implementar operações da pilha para controlar os bits de stuff na hora de enviar
typedef struct stack {
    bool data;
    struct stack* next; 
} stack;

typedef struct frame {
    int id,dlc,crc,eof;
    long data;
    bool sof,rtr,srr,ide,r0,r1,crc_delimiter,ack_slot,ack_delimeter;
}frame;

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
//general way of saying an error occurred
bool error_bit = false;

//Bits responsible for logic control of reading and writing 
bool write_bit = false;
bool received_bit = false;
bool transmited_bit = false;
// saves the last bit for ease
bool last_bit = false;
// guards all bits waiting to be transmitted
stack trm_bits;
// flag that indicates if this bit is a stuffed bit (for the receiver only)
bool bit_stuff = false;

//Auxiliar vars, mostly counters
int bit_stuff_count = 0;
int frame_count = 0;

//Var responsible for keeping track of the state of the system
states state = inter_frame_space;
can_types can_type = schrodinger_frame;
frame my_frame;


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

// essa função deve setar todas as flags de modo que todos os outros módulos
// consigam interpretar se houve erro/bit_stuffed/qualquer flag de interesse
void bit_stuff_error_detect (){
// TODO: implementar lógica de last_bit/next_bit usando uma pilhas
// TODO: Garantir que a pilha nunca esteja vazia   
// TODO: Criar os pushs na pilha   
    bool this_bit;
    this_bit = (trm_bits.next)->data;
    
    if(write_bit){
        if( this_bit == last_bit && bit_stuff_count == 5){
            bit_stuff_count = 1;
            send(!last_bit);
        }
        else if ( this_bit == last_bit){
            bit_stuff_count += 1;
            send(this_bit);
            pop();
        }
        else{
            bit_stuff_count = 0;
            send(this_bit);
            pop();
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
            bit_stuff_error == true;
        }
    }
}

//monta os pacotes para os nós que estão lendo/escrevendo(no caso de uma perda de barramento)?
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
}


// Isso faz sentido quando estiver enviando o pacote? Não, para enviar basta montar o pacote e enviar no tempo certo.
// Isso sempre checando o bit_stuff claro.
// esse é o metódo apenas quando estou lendo e não quando estou escrevendo.

void arb_phase_method (){
    bool this_bit;
    this_bit = (trm_bits.next)->data;

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