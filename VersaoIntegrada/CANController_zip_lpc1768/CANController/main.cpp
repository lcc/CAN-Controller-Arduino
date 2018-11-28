#include "mbed.h"
#include "data_frame_utils.h"
//#include "rtos.h"

//Parâmetros configuráveis
#define SJW   2
#define SYNC  1
#define TSEG1 8
#define TSEG2 7
//#define BITRATE  62.5 // Para um Tq de 1ms
//#define BITRATE  6.25 // Para um Tq de 10ms
//#define BITRATE  1// Bit com 1 segundo (debug)
#define BITRATE 0.0625//Tq de 1 segundo (debug)
#define EOFINTERFRAME 10
#define ON 1
#define OFF 0

//Serial para debug
Serial pc(USBTX,USBRX);

//Leds de sinal
DigitalOut tq_led(p19);
DigitalOut softsync_led(LED1);
DigitalOut hardsync_led(LED2);
DigitalOut st_led2(LED3);
DigitalOut st_led1(LED4);

//Saídas de sinal de indicação
DigitalOut tq_sig(p11);
DigitalOut softsync_sig(p12);
DigitalOut hardsync_sig(p13);
DigitalOut st_sig1(p14);
DigitalOut st_sig2(p15);

//Controles
DigitalIn  REQ(p5);
DigitalOut SAMPLE(p6);
DigitalOut WRITE(p7);
DigitalIn  TX(p8);
DigitalOut RX(p9);

//Interface com transciever
DigitalOut tx_can(p22);
DigitalIn  rx_can(p21);

//Timeout
Ticker tic;

//------------------------------------------------------------------------------
//Declaração dos ENUMs
//------------------------------------------------------------------------------
enum{
    SYNC_STATE,
    SEG1_STATE,
    SEG2_STATE
}STATE;

//------------------------------------------------------------------------------
//Variáveis
//------------------------------------------------------------------------------
int   tq_counter = 0;
int   s1_counter=0;
int   s2_counter=0;
int   bitstate = 0;
int   next_state = SYNC_STATE;
float tq_period = 0;
float bit_period=0;
int   last_bit_read = 1;
int   bit_read = 1;
int   recessive_bit_counter = 0;
int   low_transition_detected = 0;
int   hardsync=0;
int   softsync=0;
int   sjw=0;
int   sample=0;
int   sigwrite=0;

//------------------------------------------------------------------------------
//Função para configurar a velocidade de comunicação
//------------------------------------------------------------------------------
void configBitRate(float baud){
    
    bit_period = (float)1/(float)BITRATE;
    tq_period = (bit_period/(float)(SYNC+TSEG1+TSEG2));
    printf("tq_period: %f...\r\n",tq_period);

}

//------------------------------------------------------------------------------
//Função para o estado sync
//------------------------------------------------------------------------------
int syncState(){
    
    tq_counter=1; // incremanta contador de tq.
    tq_led = !tq_led;//Toggle do led de tq.
    tq_sig = !tq_sig;//Toogle do sinal de tq no pino.
    st_led2 = OFF;
    st_led1 = OFF;// leds de indicação de estado interno = 00(0).
    st_sig2 = 0;
    st_sig1 = 0;// sinais de indicação de estado interno = 00(0).
    sigwrite=0;
    softsync_led=0;
    hardsync_led=0;
    softsync=0;
    hardsync=0;
    
    bit_read=rx_can.read();//Leitura do pino rx da interface can
    
    if( last_bit_read == 1 ){
        
        if( bit_read == 0 ) { //detectou uma transição high to low

            if(recessive_bit_counter > EOFINTERFRAME){//Verifica se foi hard ou soft sync
                hardsync_led=1;
                hardsync=1;
                WRITE=1;
                sigwrite=1;
            }
            else{                 
                softsync_led=1;
                softsync=1;
            }
        }
    }
    
    return SEG1_STATE;

}

//------------------------------------------------------------------------------
//Função para o estado TSEG1
//------------------------------------------------------------------------------
int t1State(){
    s1_counter++;
    tq_counter=s1_counter;// incremanta contador de tq.
    tq_led = !tq_led;//Toggle do led de tq.
    tq_sig = !tq_sig;//Toogle do sinal de tq no pino.
    st_led2 = 0;
    st_led1 = 1;//  leds de indicação de estado interno = 01(1).
    st_sig2 = 0;
    st_sig1 = 1;//  sinais de indicação de estado interno = 01(1).
    sigwrite=0;
    softsync_led=0;
    hardsync_led=0;
    softsync=0;
    hardsync=0;
        
    bit_read=rx_can.read();//Leitura do pino rx da interface can
    
    if( last_bit_read == 1 ){
        
        if( bit_read == 0 ){ //detectou uma transição high to low

            if( recessive_bit_counter > EOFINTERFRAME ){//Verifica se foi hard ou soft sync
                hardsync_led=1;
                hardsync=1;
                sigwrite=1;
                WRITE=1;
                s1_counter=0;
                last_bit_read=0;
                recessive_bit_counter=0;
            }
            else{
                softsync_led=1;
                softsync=1;
                sjw=SJW;
            }
        }
    }
    
    if( s1_counter < TSEG1+sjw ){//fica nesse estado até que s1_counter chegue em TSEG1+sjw
        return SEG1_STATE;
    }
    else{//Senão salta para o próximo estado
        s1_counter=0;
        sjw=0;
        return SEG2_STATE;
    }

}

//------------------------------------------------------------------------------
//Função para o estado TSEG2
//------------------------------------------------------------------------------
int t2State(){
    s2_counter++;
    tq_counter=s2_counter;// incremanta contador de tq.
    tq_led = !tq_led;//Toggle do led de tq.
    tq_sig = !tq_sig;//Toogle do sinal de tq no pino.
    st_led2=1;
    st_led1=0;//leds de indicação de estado interno = 10(2).
    st_sig2 = 1;
    st_sig1 = 0;//  sinais de indicação de estado interno = 10(2).
    sample = 0;
    softsync_led=0;
    hardsync_led=0;
    softsync=0;
    hardsync=0;
    WRITE=0;
    sigwrite=0;
    
    bit_read=rx_can.read();//Leitura do pino rx da interface can    
    
    if( last_bit_read == 1 ){
        
        if( bit_read == 0 ){ //detectou uma transição high to low
   
            if(recessive_bit_counter > EOFINTERFRAME){//Verifica se foi hard ou soft sync
                hardsync_led=1;
                hardsync=1;
                WRITE=1;
                sigwrite=1;
                last_bit_read=0;
                recessive_bit_counter=0;             
            }
            else{
                softsync_led=1;
                softsync=1;
                sjw=(-1)*SJW;
            }
        }
        
    }
    
    if( s2_counter == 1 ){
        SAMPLE=1;
        sample=1;
        RX=rx_can;
        last_bit_read = bit_read;
        if(bit_read==1){
            recessive_bit_counter++;
        }
        else{
            recessive_bit_counter=0;
        }
    }
    else{
        SAMPLE=0;
    }

    if( hardsync==1 ){
            sjw=0;
            s2_counter = 0;
            return SEG1_STATE;
    }
    if( s2_counter < (TSEG2+sjw) ){
        return SEG2_STATE;
    }
    else{
            sjw=0;
            s2_counter=0;
            return SYNC_STATE;
    }

}

//------------------------------------------------------------------------------
//Função que imprime os sinais internos na serial
//------------------------------------------------------------------------------
void serial_debug(){
    
    pc.printf("tq_counter:%2d  ",tq_counter);
    pc.printf("state:%2d  ",bitstate);
    pc.printf("next_state:%2d  ",next_state);
    pc.printf("bit_read:%2d  ",bit_read);
    pc.printf("last_bit_read:%2d  ",last_bit_read);
    pc.printf("hardsync:%2d  ",hardsync);
    pc.printf("softsync:%2d  ",softsync);
    pc.printf("recessive_bit_counter:%2d  ",recessive_bit_counter);
    pc.printf("sample:%2d  ",sample);
    pc.printf("write:%2d  ",sigwrite);
    pc.printf("\r\n");
    
}

//------------------------------------------------------------------------------
//Função que imprime os sinais internos na serial
//------------------------------------------------------------------------------
void atTic(){
    
    switch(next_state){
            
        case SYNC_STATE:
            bitstate=0;
            next_state = syncState();
        break;
        
        case SEG1_STATE:
            bitstate=1;
            next_state = t1State();
        break;
        
        case SEG2_STATE:
            bitstate=2;
            next_state = t2State();
        break;
        
    }        
        
    //serial_debug();
        
}

//------------------------------------------------------------------------------
//Função principal
//------------------------------------------------------------------------------
int main() {
    bool a=0;
    pc.baud(921600);
    configBitRate(BITRATE);
    tic.attach(&atTic,tq_period);
    //tic.attach(&atTic,1);
    tx_can=1;
    write_bit = 1;
    setting_things_up();
    
    while(state!=idle) {
        pc.printf("state: %d",state);
        a=encoder();
        REQ.read();
                
        if(REQ ==1){
            tx_can=TX;
        }
        
    }
    
}