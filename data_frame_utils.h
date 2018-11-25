#include <iostream>
#include <stdlib.h> 
#include <cmath>
#include <bitset>
using namespace std;

typedef enum {SOF,arb_phase,control_field,data_field,CRC,ACK,EOFR,overload,error,inter_frame_space} states;
typedef enum {schrodinger_frame,can_A,can_B} can_types;

// transformed eof into a bool for praticity
typedef struct frame {
    int id,id2,dlc,crc;
    long data;
    bool sof,rtr,srr,ide,r0,r1,crc_delimiter,ack_slot,ack_delimeter,eof;
}frame;

// Standard values for the CAN protocol
// can_A_arb_length =  ID + RTR = 1 + 11 
// can_A_arb_length = ID + SRR + IDE + ID_2 + RTR = 11 + 1 + 1 + 18 + 1 =

// Flags responsible for triggering errors
extern bool bit_stuff_error ;
extern bool start_seven_recessive_error ;
extern bool crc_error ;

//general way of saying an error occurred, will help by 
//not having a gigantic bool expression to check wheter
// an error occurred
extern bool error_bit ;

//Bits responsible for logic control of reading and writing 
extern bool write_bit ;
extern bool received_bit ;
extern bool transmited_bit ;
// saves the last bit for ease
extern bool last_bit ;
// guards all bits waiting to be transmitted
// flag that indicates if this bit is a stuffed bit (for the receiver only)
extern bool bit_stuff;

//Auxiliar vars, mostly counters
extern int bit_stuff_count;
extern int frame_count;
extern int count;
//Var responsible for keeping track of the state of the system
extern states state;
extern can_types can_type;
extern frame my_frame;

//----------------------------Functions Declarations---------------------------------------

// essa função deve setar todas as flags de modo que todos os outros módulos
// consigam interpretar se houve erro/bit_stuffed/qualquer flag de interesse
void bit_stuff_error_detect();
// monta os pacotes para os nós que estão lendo/escrevendo(no caso de uma perda de barramento)?
// talvez haja um jeito melhor, guardar o bit caso perca e ir montando o frame daí!
void packet_mount (bool this_bit);

// Isso faz sentido quando estiver enviando o pacote? Não, para enviar basta montar o pacote e enviar no tempo certo.
// Isso sempre checando o bit_stuff claro.
// esse é o metódo apenas quando estou lendo e não quando estou escrevendo.
void arb_phase_method ();

//Sets all variables to it's inital values
void setting_things_up ();
void my_frame_mount();
void my_frame_zeros();
void arb_phase_send_logic();
void control_field_send_logic();
void data_field_send_logic();
void crc_field_send_logic();
void ack_field_send_logic();
void eof_field_send_logic();
bool set_bit_send();