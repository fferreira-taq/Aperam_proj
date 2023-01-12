/*
 * Robot Teste APERAM.
 *
 * UIPEthernet is a UDP stack that can be used with a enc28j60 based
 * Ethernet-shield.
 *
 * UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>
 *
 *      -----------------
 *
 * This UdpServer example sets up a udp-server at 192.168.0.169 on port 8888.
 * send packet via upd to test
 *
 * Copyright (C) 2013 by Norbert Truchsess (norbert.truchsess@t-online.de)
 */

#include <SPI.h>
#include <UIPEthernet.h>

EthernetUDP udp;

// Define Pinos
const int DDP = A0;

const int FC_TH1  = 30;
const int FC_TH2  = 31;
const int FC_BH1  = 32;
const int FC_BH2  = 33;
const int FC_EH1  = 34;
const int FC_EH2  = 35;
const int FC_RH1  = 36;
const int FC_RH2  = 37;
const int FC_M1   = 38;
const int FC_M2   = 39;

const int MH_H1V   = 40;
const int MH_H2V   = 41;
const int MAH_H1V  = 42;
const int MAH_H2V  = 43;
const int MH_H1H   = 44;
const int MH_H2H   = 45;
const int MAH_H1H  = 46;
const int MAH_H2H  = 47;
const int MH_M     = 48;
const int MAH_M    = 49;

void setup() {
  
  // Configura Pinos
  pinMode(FC_TH1, INPUT_PULLUP);
  pinMode(FC_TH2, INPUT_PULLUP);
  pinMode(FC_BH1, INPUT_PULLUP);
  pinMode(FC_BH2, INPUT_PULLUP);
  pinMode(FC_EH1, INPUT_PULLUP);
  pinMode(FC_EH2, INPUT_PULLUP);
  pinMode(FC_RH1, INPUT_PULLUP);
  pinMode(FC_RH2, INPUT_PULLUP);
  
  pinMode(MH_M, OUTPUT);
  pinMode(MAH_M, OUTPUT);
  pinMode(MH_H1V, OUTPUT);
  pinMode(MAH_H1V, OUTPUT);
  pinMode(MH_H2V, OUTPUT);
  pinMode(MAH_H2V, OUTPUT);
  pinMode(MH_H1H, OUTPUT);
  pinMode(MAH_H1H, OUTPUT);
  pinMode(MH_H2H, OUTPUT);
  pinMode(MAH_H2H, OUTPUT);
  
  //Inicia Serial
  Serial.begin(9600);

  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
  // Inicia Ethernet
  Ethernet.begin(mac,IPAddress(192,168,0,169));

  int success = udp.begin(9999);

  Serial.print("initialize: ");
  Serial.println(success ? "success" : "failed");

}

void loop() {

  //Confere chegada de pacotes UDP:
  boolean ChS = false;
  unsigned int cmd;
  
  int size = udp.parsePacket();
  if (size > 0) {
    do{
        char* msg = (char*)malloc(size+1);
        int len = udp.read(msg,size+1);
        msg[len]=0;
        
   // checksum
        int Sum = 0;
        if(len == 3)
          {
          for(int i = 0; i < (len-1); i++){
            Sum += msg[i];
          }
          if (Sum == msg[len-1])
            ChS = true;
            
          }else{ChS = false;}
     // fim check sum
     
     // Se o checkSum esta correto armazena comandos
      if(ChS)
       cmd = ((msg[0]<<8)&0xFF00)+(0x00FF&msg[1]);
       
      free(msg);
    }while ((size = udp.available())>0);
        
    //finish reading this packet:
    udp.flush();
    
    int success;
    char Status[4];
    
    // Compoes resposta Ack: Status da Potencia|Ack|Status CheckSum
    Status[0] = (0x80)+(0x40 & (true<<6))+(0x20 & (ChS<<5));
    Status[1] = 0;
    Status[2] = 0;
    Status[3] = 0;
    // CheckSum
    for (int i=0; i<3; i++)
    {
     Status[3] += Status[i];  
    }
       
    // Inicia resposta UDP
    do{
        Serial.println(udp.remoteIP());
        success = udp.beginPacket(udp.remoteIP(),8888);
      }while (!success);

    // Corpo da Mensagem
    success = udp.print(Status[0]);
    success = udp.print(Status[1]);
    success = udp.print(Status[2]);
    success = udp.print(Status[3]);

    
    success = udp.print("'");
    success = udp.print(Status[0],HEX);
    success = udp.print(":");   
    success = udp.print(Status[1],HEX);
    success = udp.print(":"); 
    success = udp.print(Status[2],HEX);
    success = udp.print(":"); 
    success = udp.print(Status[3],HEX);
    success = udp.println("'");
      
//    success = udp.println("Ack");
//    success = udp.print("CheckSum:");
//    success = udp.println(ChS ? "ON" : "OFF");

// Finaliza Mensagem UDP    
    success = udp.endPacket();    
    
// Se o CheckSum esta correto 
    if(ChS)
    { // faz a leitura dos sensores e atua no robot
      int TL_haste1 = digitalRead(FC_TH1);
      int TL_haste2 = digitalRead(FC_TH2);
      int BL_haste1 = digitalRead(FC_BH1);
      int BL_haste2 = digitalRead(FC_BH2);  
      int EL_haste1 = digitalRead(FC_EH1);
      int EL_haste2 = digitalRead(FC_EH2);
      int RL_haste1 = digitalRead(FC_RH1);
      int RL_haste2 = digitalRead(FC_RH2);
      int Manobra1 = digitalRead(FC_M1);
      int Manobra2 = digitalRead(FC_M2);
      int Tensao   = analogRead(DDP);


// Atua no Robo segundo comando recebido
switch (cmd) {

  case 0xC000:
    // Direcional UP
    if((!Manobra1)&&(!Manobra2))
      digitalWrite(MH_M,HIGH);
    delay(100);
    break;
  case 0xA000:
    // Direcional DOWN 
    if((!Manobra1)&&(!Manobra2))
      digitalWrite(MH_M,HIGH);
    delay(100);
     break;
  case 0x9000:
    // Manobra    
    if(!Manobra1)
      digitalWrite(MH_M,HIGH);
    delay(100);
     break;
  case 0x8800:
    // Haste1 UP    
    if(!TL_haste1)
      digitalWrite(MH_H1V,HIGH);
    delay(100);
     break;
  case 0x8400:
    // Haste2 UP    
    if(!TL_haste2)
      digitalWrite(MH_H2V,HIGH);
    delay(100);
    break;
  case 0x8200:
    // Haste1 DOWN    
    if(!BL_haste1)
      digitalWrite(MAH_H1V,HIGH);
    delay(100);
    break;
  case 0x8100:
     // Haste2 DOWN    
    if(!BL_haste2)
      digitalWrite(MAH_H2V,HIGH);
    delay(100);
    break;
  case 0x8080:
    // Haste1 EXPANDE   
    if((!EL_haste1))
      digitalWrite(MH_H1H,HIGH);
    delay(100);
     break;
 case 0x8040:
    // Haste2 EXPANDE     
    if((!EL_haste2))
      digitalWrite(MH_H2H,HIGH);
    delay(100);
    break;
 case 0x8020:
    // Haste1 RECOLHE     
    if((!RL_haste1))
      digitalWrite(MAH_H1H,HIGH);
    delay(100);
    case 0x8010:
    // Haste2 RECOLHE     
    if((!RL_haste2))
      digitalWrite(MAH_H2H,HIGH);
    delay(100);
    case 0x4121:
   // Case TESTE    
     Serial.println("teste: ok");
    delay(100);
   break;
  default: 
    // statements
  break;
}


// Compoes a Resposta de Status Completa
    Status[0] = ((Status[0] & 0xE0) + (Manobra1<<4))+(0x08 & (Manobra2<<3))+(0x04 & (TL_haste1<<2))+(0x02 & (TL_haste2<<1))+(0x01 & (BL_haste1));
    Status[1] = (0x80 & (BL_haste2<<7))+(0x40 & (EL_haste1<<6))+(0x20 & (EL_haste2<<5))+(0x10 & (RL_haste1<<4))+(0x08 & (RL_haste2<<3))+(0x04 & false)+(0x02 & (Tensao>>8))+(0x01 & Tensao>>8);
    Status[2] = Tensao & 0xFF;
    Status[3] = 0;
    
    //CheckSum
    for (int i=0; i<3; i++)
    {
      Status[3] += Status[i];  
    }
    
//  Inicia Mensagem de resposta
      do{
          Serial.println(udp.remoteIP());
          success = udp.beginPacket(udp.remoteIP(),8888);
        }while (!success);
 
 // Corpo da Mensagem de Resposta   
      success = udp.print("'");
      success = udp.print(Status[0],HEX);
      success = udp.print(":");   
      success = udp.print(Status[1],HEX);
      success = udp.print(":"); 
      success = udp.print(Status[2],HEX);
      success = udp.print(":"); 
      success = udp.print(Status[3],HEX);
      success = udp.println("'");
    
//      success = udp.print("Top Limit haste 1:");
//      success = udp.println(TL_haste1 ? "ON" : "OFF");
//      success = udp.print("Botton Limit haste 1:");    
//      success = udp.println(BL_haste1 ? "ON" : "OFF");
//      success = udp.print("Expand Limit haste 1:");    
//      success = udp.println(EL_haste1 ? "ON" : "OFF");
//      success = udp.print("Retract Limit haste 1:");    
//      success = udp.println(RL_haste1 ? "ON" : "OFF");
//      success = udp.print("Top Limit haste 2:");    
//      success = udp.println(TL_haste2 ? "ON" : "OFF");
//      success = udp.print("Botton Limit haste 2:");    
//      success = udp.println(BL_haste2 ? "ON" : "OFF");
//      success = udp.print("Expand Limit haste 2:");    
//      success = udp.println(EL_haste2 ? "ON" : "OFF");
//      success = udp.print("Retract Limit haste 2:");    
//      success = udp.println(RL_haste2 ? "ON" : "OFF");
//      success = udp.print("Valor Tensao:");    
//      success = udp.println(analogRead(DDP));
// Final da mensagem de resposta
      success = udp.endPacket();
    }
    
    udp.stop();
    //restart with new connection to receive packets from other clients
    Serial.print("restart connection: ");
    Serial.println (udp.begin(9999) ? "success" : "failed");
  }

    // desaciona tudo 
   digitalWrite(MH_H1V,LOW);
   digitalWrite(MH_H2V,LOW);
   digitalWrite(MAH_H1V,LOW);
   digitalWrite(MAH_H2V,LOW);
   digitalWrite(MH_H1H,LOW);
   digitalWrite(MH_H2H,LOW);
   digitalWrite(MAH_H1H,LOW);
   digitalWrite(MAH_H2H,LOW);
   digitalWrite(MH_M,LOW);
   digitalWrite(MAH_M,LOW);
   delay(100);
       
}
