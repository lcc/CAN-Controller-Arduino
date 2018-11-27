#include <iostream>
#include <stdlib.h> 
#include <cmath>
#include <bitset>
using namespace std;

typedef enum {SOF,arb_phase,control_field,data_field,CRC,ACK,EOFR,overload,error,inter_frame_space,idle} states;
typedef enum {schrodinger_frame,can_A,can_B} can_types;

// transformed eof into a bool for praticity
typedef struct frame {
    int id,id2,dlc,crc;
    long data;
    bool sof,rtr,srr,ide,r0,r1,crc_delimiter,ack_slot,ack_delimeter,eof;
}frame;


// Flags responsible for triggering errors
extern bool bit_stuff_error;
extern bool start_seven_recessive_error;
extern bool crc_error;
extern bool form_error;

//Bits responsible for logic control of reading and writing 

extern bool send_ack;
extern bool write_bit;
extern bool this_bit;
extern bool last_bit;
extern bool bit_stuff;

//Auxiliar vars, mostly counters
extern int bit_stuff_count;
extern int count;
extern int tail_count;

//Var responsible for keeping track of the state of the system
extern states state;
extern states error_state;
extern can_types can_type;
extern frame my_frame;

//----------------------------Functions Declarations---------------------------------------

// essa função deve setar todas as flags de modo que todos os outros módulos
// consigam interpretar se houve erro/bit_stuffed/qualquer flag de interesse
void bit_stuff_error_detect();


//Sets all variables to it's inital/specified values
void setting_things_up ();
void my_frame_mount();
void my_frame_zeros();

void state_name(states curr_state);


// functions used to select the specific bit to send
bool set_bit_send();
void arb_phase_send_logic();
void control_field_send_logic();
void data_field_send_logic();
void crc_field_send_logic();
void ack_field_send_logic();
void eof_field_send_logic();
void error_frame_logic();
void overload_frame_logic();
void inter_frame_space_logic();

// functions used to mount the package when reading
void mount_package(bool bit);
void arb_field_mount(bool bit);
void control_field_mount(bool read_bit);
void data_field_mount(bool read_bit);
void crc_field_mount(bool read_bit);
void ack_field_mount(bool read_bit);
void eof_field_mount(bool read_bit);
void inter_frame_space_check(bool read_bit);

// functions that run to assist the mounting/sending of messages
void bit_stuff_logic(bool read_bit);
void dlc_correction();

void change_mode(bool read_bit);
bool encoder();
void decoder(bool read_bit);


