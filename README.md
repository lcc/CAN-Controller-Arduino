# CAN-Controller-Arduino
This is an arduino project that simulates an ECU running the CAN 2.0A &amp; 2.0B protocol. 

TODO: 

- Errors:
    - Bit_error         [] -->  if (RX != TX && state > arb_phase)
    - Bit_Stuff error   [x]
    - Form error        [x]
        - CRC delimiter [x]
        - ACK delimiter [x]
        - End of Frame  [x]
        - Intermission  [x]

    - Ack_error         [x] --> adicionar isso em cima de decoder() if(state == ack && send_ack){write = 0;} ====> Vou setar send_ack pra 0 quando chamar read;
    - CRC_error         []

- 

- Add Overload and Error state methods                        [x]
- Se frame for request -> não ter DATA -> passar direto para crc  => resolvi isso colocando no data_frame um check pra ver se o bit rtr ou o srr
- Se DLC for zero-> passsar direto para crc 
- Criar uma função raise_error                                [x]
- Escrever o bit de ack                                       []
    - Posso criar um ack_del_t e escrever 0                   []
- Calculo do CRC juntamente com a verificação dele            []
- Adicionar e testar super-módulo com bit-stuffing            [x]
- Desenvolver lógica para a perda de uma arbitração           [x]
- Integrar esse módulo com o código de saulo                  []
- verificar se settingthingsup está funcionando adequadamente []
