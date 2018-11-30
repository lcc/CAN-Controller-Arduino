#include "data_frame_utils.h"
#include <iostream>
#include<string.h>  
using namespace std;

void debbug_myframe_can_B();
void debbug_myframe_can_A();
void print_myframe_can_A();
void print_myframe_can_B();
void print_frame_data();
void testing_send_functions();
void testing_received_functions();
void erro_debbuger();
void test_frames();
bool char_to_bool(bool a);

int main (){
    test_frames();    
    //testing_received_functions();
    //testing_send_functions();
    return 0;
}

void testing_received_functions(){
    int quant = 0;
    bool reade_bit = false;
    setting_things_up();
    while(quant){
        cout << '\n' << quant << "\n";
        while(state != idle && state != erro && state != overload){
            reade_bit = rand() % 2;            
            decoder(reade_bit);
            cout << reade_bit;
       }
        cout << "\n";
        quant--;
        erro_debbuger();
        if(!my_frame.ide ){debbug_myframe_can_A(); cout <<"Can A" << "\n";}
        else if (my_frame.ide){debbug_myframe_can_B(); cout << "Can B" << "\n";}
        write_bit = false;
        setting_things_up();
    }
}

void erro_debbuger (){
    if(bit_stuff_erro){
        cout << "-------Stuff_erro-------" << "at state: ";
        state_name(erro_state);
    }
    if(form_erro){
        cout << "-------Form_erro-------" << "at state: ";
        state_name(erro_state);
    }

}

void testing_send_functions (){
    
    int quant = 20;
    bool reade_bit = false;
    write_bit = 1;
    setting_things_up();
    //cout << my_frame.id << "\n";
    //cout << '\n' << my_frame.crc << "\n";
    if(my_frame.ide == 0){
        cout << "Can_A"  << "\n";
        debbug_myframe_can_A();
    }
    else{
        cout << "Can_B" << "\n";
        debbug_myframe_can_B();
    }

    while(quant){
        
        reade_bit = encoder();
        
        cout  << reade_bit;
        std::cout.flush();
        if(state == inter_frame_space){
            cout << "\n";
            setting_things_up();
            quant--;
            if(my_frame.ide == 0){
                cout << "Can_A"  << "\n";
                debbug_myframe_can_A();
            }
            else{
                cout << "Can_B" << "\n";
                debbug_myframe_can_B();
            }
        }
    }
    //cout << "\n" << my_frame.id << " " << (my_frame.srr | my_frame.rtr) << " " << my_frame.ide << " " << my_frame.id2 << '\n';
    //cout << '\n' << my_frame.dlc << ' ' << my_frame.data << '\n';
    //cout << '\n' << my_frame.r0 << " " << my_frame.r1 << " " << my_frame.dlc << "\n";
}

void print_frame_data(){
    if(!my_frame.rtr){
        for (int i = my_frame.dlc*8; i > 0; i--)
            std::cout << ((my_frame.data >> i) & 1);
    }
}

void debbug_myframe_can_B (){
    cout << "CAN_B \n";
    cout << my_frame.sof;
    std::cout << std::bitset<11>(my_frame.id);        
    cout << my_frame.srr;
    cout << my_frame.ide;
    std::cout << std::bitset<18>(my_frame.id2);        
    cout << my_frame.rtr;
    cout << my_frame.r1;
    cout << my_frame.r0;
    std::cout << std::bitset<4>(my_frame.dlc);
    print_frame_data();
    std::cout << std::bitset<15>(my_frame.crc);
    cout << my_frame.crc_delimiter;
    cout << my_frame.ack_slot;
    cout << my_frame.ack_delimeter;
    for(int i = 0; i<7;i++)
        cout << my_frame.eof;
    cout << "\n";
}

void debbug_myframe_can_A (){
    cout << "CAN_A \n";
    cout << my_frame.sof;
    std::cout << std::bitset<11>(my_frame.id);        
    cout << my_frame.rtr;
    cout << my_frame.ide;
    cout << my_frame.r0;
    std::cout << std::bitset<4>(my_frame.dlc);
    print_frame_data();
    std::cout << std::bitset<15>(my_frame.crc);
    cout << my_frame.crc_delimiter;
    cout << my_frame.ack_slot;
    cout << my_frame.ack_delimeter;
    for(int i = 0; i<7;i++)
        cout << my_frame.eof;
    cout << "\n";
}


bool char_to_bool(char a){
    if(a == '1'){return 1;}
    else return 0;
}




void test_received_frames (){
    string frame;
    bool b;

    string frame_test[31] = {"0110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011011111111",
"011001110010000011111001010101010101010101010101010101010101010101010101010101011001100111011011111111",
"011001110010000011101010101010101010101010101010101010101010101010100001001001011101011111111",
"0110011100100000110110101010101010101010101010101010101010101100111001001101011111111",
"01100111001000001100101010101010101010101010101010101110111010100101011111111",
"0110011100100000101110101010101010101010101001001011100000111011111111",
"0110011100100000101010101010101010100110110111100111011111111",
"01100111001000001001101010101100111000010101011111111",
"011001110010000010000110010110101011011111111",
"0110011100101000001010000011000100001011111111",
"011001110010100000110000100100010011011111111",
"0100010010011111000001000001111100100001000101010101010101010101010101010101010101010101010101010101010101011110111110101011011111111",
"010001001001111100000100000111110010100100001010011111001101011111111",
"0100010010011111000001000001111100101000001010111011111000011011111111",
"0100010010011111000001000001111100100000101110101010101010101010101000011111001011101011111111",
"010001001001111100000100000111110010000010000101110001101001011111111",
"0110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011100000011111111",
"011001110010000100010101010101010101010101010101010101010101010101010101010101010100000000000011111111",
"01100111001000010001010101010101010101010101010101010101010101010101010101010101010000010000111000110100000011111111",
"011001110010000100010101010101010101010101010101010101010101010101010101010101010100000100001010001110000001111111100000011111111",
"0110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011011111111110110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011011111111",
"011001110010000100010101010101010101010101010101010101010101010101010101010101010100000100001010001101111111110000000111111111110110011100100001000101010101010101010101010101010101010101010101010101010101010101000001000010100011011111111",
"0000010000011111000001000001000001000001001111101111101111101111101111101111101111101111101111101111101111101111101111101110101100001011111011111111",
"0100010010010100000000000011111111",
"010001001001111100000100000111110010000010111010101010101010101010100001111100101110101000000011111111",
"01000100100111110000010000011111001000001011101010101010101010101010000111110010111010000000011111111",
"010001001001111100000100000111110010000010111010101010101010101010100001111100101110000000011111111",
"0100010010011111000001000001111100100000101110101010101010101010101000011111001011101011111111000000111111111101000100100111110000010000011111001000001011101010101010101010101010000111110010111010111111111000000011111111110100010010011111000001000001111100100000101110101010101010101010101000011111001011101011111111000010000100000011111111110100010010011111000001000001111100100000101110101010101010101010101000011111001011101011111111"
};
    for(int i=0; i<37;i++){
        cout << std::dec << "Frame: " << i+1 << endl;
        frame = frame_test[i];
        write_bit = false;
        setting_things_up();
        for(int i =0; frame[i] != '\0'; i++){
            b = char_to_bool(frame[i]);
            decoder(b);
            //state_name(state);
        }
        if(!my_frame.ide){
            debbug_myframe_can_A();
       }
        else {debbug_myframe_can_B();}
       
        
        if(!my_frame.ide){print_myframe_can_A();}
        if(my_frame.ide){print_myframe_can_B();}

    }
}



void print_myframe_can_A (){
    cout << "SOF: " << my_frame.sof << " ";
    cout << "ID: " << std::hex << my_frame.id << " ";        
    cout << "RTR: " << my_frame.rtr << " ";
    cout << "IDE: "<< my_frame.ide << " ";
    cout << "r0:" << my_frame.r0 << " ";
    cout << "DLC: " << my_frame.dlc << " ";
    cout << "DATA: " << std::hex << my_frame.data  << " ";
    std::cout << "CRC: " << std::bitset<15>(my_frame.crc) << " ";   
    cout << "CRC_del: " << my_frame.crc_delimiter << " ";
    cout << "ACK_SLOT: " << my_frame.ack_slot << " ";
    cout << "ACK_DEL: " << my_frame.ack_delimeter << " ";
    cout << "EOF: ";    
    for(int i = 0; i<7;i++)
        cout << my_frame.eof << " ";
    cout << "\n";
    if(state == erro)
        erro_debbuger();
    cout << "\n";
}

//(15)ID_A - 0x0449 srr = 1 ide =1 id_b =0x3007A (stuff)  rtr =0 dlc = 3  data = AAAAAA   CRC = 000111110101110 (ACK=notok)(até EOF)


void print_myframe_can_B (){
    cout << "SOF: "<< my_frame.sof << " ";
    cout << "ID: " << std::hex << my_frame.id << " ";        
    cout << "SRR: "<<  my_frame.srr << " ";
    cout << "IDE: "<< my_frame.ide << " ";
    cout << "ID2: " << std::hex << my_frame.id2 << " ";
    cout << "RTR: " << my_frame.rtr << " ";                
    cout << "r1: " << my_frame.r1 << " ";
    cout << "r0: "<< my_frame.r0 << " ";
    cout << "DLC: "<< my_frame.dlc << " ";
    cout << "DATA: "<< std::hex << my_frame.data  << " ";
    std::cout << "CRC: " << std::bitset<15>(my_frame.crc) << " ";   
    cout << "CRC_del: " << my_frame.crc_delimiter << " ";
    cout << "ACK_SLOT: " << my_frame.ack_slot << " ";
    cout << "ACK_DEL: " <<  my_frame.ack_delimeter << " ";
    cout << "EOF: ";
    for(int i = 0; i<7;i++)
        cout << my_frame.eof << " ";
    cout << "\n";
    if(state == erro)
        erro_debbuger();
    cout << "\n";
}





