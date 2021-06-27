/*------------------------------------------------------------------------*/
/* Título: Experimento 15
 * Autor: Yan Podkorytoff Ike Chícharo
 * Descrição: O objetivo desse experimento é implementar a codificação manchester
 */
/*------------------------------------------------------------------------*/
#include <string.h>
#define bit_period 100
#define SET_BIT() PORTD |= (1<<4)
#define CLR_BIT() PORTD &= ~(1<<4)
#define readPin A3
//#define STX 0x02
//#define ETX 0x03

int incomingByte;
int counter = 0;
int i = 0;
int jobDone = 0;
int frame_length;
char STX = 0x02;
char msg[] = "OI  KENJI";
char ETX = 0x03;

void emit_bit(char *data){
  //SEND MSG
  do{
    do{
      if((data[i] >> counter) & 0x01){
         SET_BIT();
         delay(bit_period);
         incomingByte = analogRead(readPin);
         Serial.println(incomingByte);
         CLR_BIT();
         incomingByte = analogRead(readPin);
         Serial.println(incomingByte);
         Serial.print("contador: ");
         Serial.println(counter);
       }else{
         CLR_BIT();
         delay(bit_period);
         incomingByte = analogRead(readPin);
         Serial.println(incomingByte);
         SET_BIT();
         incomingByte = analogRead(readPin);      
         Serial.println(incomingByte);
         Serial.print("contador: ");
         Serial.println(counter);
       }
       counter++;
       delay(bit_period);
    }while(counter<8);
    i++;
    counter = 0; 
  //}while(i < (sizeof(data)/sizeof(char)));
  }while(i < 9);
}

void setup() {
  pinMode(A3,INPUT);
  Serial.begin(9600);
  DDRD |= (1<<4);
}

void loop() {
  if(jobDone == false){
    delay(2000);
    emit_bit(msg);
    CLR_BIT();
    jobDone = true;
  }
}
