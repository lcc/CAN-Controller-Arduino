#include "data_frame_utils.h"

void debbug_myframe_can_B();
void debbug_myframe_can_A();
void print_frame_data();
void testing_send_functions();
void testing_received_functions();
void error_debbuger();

// no stuffing = (CRC_DELIMITER,ACK FIELD, END_OF_FRAME)
int main (){
    testing_send_functions();
    return 0;
}

void testing_received_functions(){
    bool reade_bit = false;
    int quant = 100;
    setting_things_up();
    while(quant){
        cout << '\n' << quant << "\n";
        while(state != idle && state != error){
            reade_bit = rand() % 2;
            //daqui até o resto do while == lógica do bit_stuff pra quem ta lendo
            if(!bit_stuff)mount_package(reade_bit);

            if(SOF <= state < ACK){bit_stuff_logic(reade_bit);}

            if(bit_stuff && reade_bit == last_bit){
                error_state = state;
                state = error;
                //write_bit = 1;
                cout << reade_bit;
                break;
            }
            else if(bit_stuff && reade_bit != last_bit){bit_stuff = false;bit_stuff_count= 1;}
            //revisar até  aqui
       }
        cout << "\n";
        quant--;
        error_debbuger();
        if(!my_frame.ide && state != error){debbug_myframe_can_A();}
        else if (my_frame.ide && state != error){debbug_myframe_can_B();}

        setting_things_up();
    }
}

void error_debbuger (){
    if(bit_stuff_error){
        cout << "-------Stuff_error-------" << "at state: ";
        state_name(error_state);
    }
    if(form_error){
        cout << "-------form_error-------" << "at state: ";
        state_name(error_state);
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
        if(!bit_stuff){reade_bit = set_bit_send();}
        else{reade_bit = !last_bit; bit_stuff = false;bit_stuff_count=1;}
        if(SOF <= state < ACK){bit_stuff_logic(reade_bit);}        
        
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
    for (int i = my_frame.dlc*8-1; i >= 0; i--)
        std::cout << ((my_frame.data >> i) & 1);
}

void debbug_myframe_can_B (){
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

//1 01000110011 0000001110010010000000000000001101111111
//1 01000110011 1000001110010010000000000000001101111111