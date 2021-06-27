/*------------------------------------------------------------------------*/
/* Título: Experimento 14
 * Autor: Yan Podkorytoff Ike Chícharo
 * Descrição: O objetivo desse experimento é implementar a codificação manchester
 */
/*------------------------------------------------------------------------*/

#include <TimerOne.h>
#define RX_PIN A3
#define FS 25000 //0.025 segundos
#define n_samples 4
#define frame_length 144 //como o sinal vem codificado em manchester, devemos considerar o dobro

uint16_t rx_value;
volatile int overflows = 0;
volatile int samples_counter = 0;
//int samples_number = 4;
volatile int average_signal = 600;
volatile uint16_t samples_buffer[n_samples];
volatile int bit_received = 0;
volatile int frame_buffer[frame_length];
int msg = 0x00;
int k = 1;
int h = 0;
void setup()
{
  Serial.begin(9600);
  pinMode(A3, INPUT);
  pinMode(3, INPUT_PULLUP);
  ADC_init();
  attachInterrupt(digitalPinToInterrupt (3), sigdetect, RISING);   
  Timer1.initialize(FS);
  Timer1.stop();
  Timer1.attachInterrupt(timerOneISR);  
}

void loop()
{
  if(overflows>(frame_length*n_samples)){
    noInterrupts();
    Timer1.stop();
    overflows = 0;
    EIFR  |= (1<<INTF1);
    attachInterrupt(digitalPinToInterrupt (3), sigdetect, RISING);
    cli();
    interrupts(); 
    bit_received = 0;   
    for(int j = 0; j < frame_length; j++){
      frame_buffer[j] = 0;
    }
    h = 0;
    k = 0;
  }
  if(samples_counter==4){
    quantizer(n_samples);
    samples_counter = 0;
  }
  
  if(bit_received == 16*k){
    reconstruct_msg();
    k++;
  }
}

void quantizer(int i)
{
   float avg = 0;
   int sum = 0;
   for(i = 0; i < 3; i++){
    sum += samples_buffer[i];
   }
   avg = float(sum)/i;
   if(avg >= average_signal){
    frame_buffer[bit_received] = 1;
    //Serial.println(frame_buffer[bit_received]);
    bit_received++;
   }else{
    frame_buffer[bit_received] = 0;
    //Serial.println(frame_buffer[bit_received]);
    bit_received++;
   }
}

void timerOneISR()
{
  samples_buffer[samples_counter] = analogRead(A3);
  overflows++;
  samples_counter++;
}

void sigdetect()
{
  //Serial.println("Sinal detectado");
  for(int j = 0; j < frame_length; j++){
      frame_buffer[j] = 0;
   } 
  samples_counter = 0;
  detachInterrupt(digitalPinToInterrupt (3));
  Timer1.restart();
}

void ADC_init()
{
  ADMUX = (1 << REFS0);
  ADCSRA |= (1 << ADEN) | (0 << ADSC) | (0 << ADATE);
  ADCSRB = 0x00;
}

uint16_t adc_read(uint8_t ch)
{
  ADCSRA |= (1 << ADSC);
  while (ADCSRA & (1 << ADSC));
  ADC = (ADCL | (ADCH << 8));
  return (ADC);
}

/*bool decoder(int index){
  if((frame_buffer[index + 16*h] == 1) && (frame_buffer[index + 1 + 16*h] == 0)){
    return true;
  }else if((frame_buffer[index + 16*h] == 0) && (frame_buffer[index + 1 + 16*h] == 1)){
    return false;
  }  
}
*/
//(frame_buffer[i + 16*h] == 1) && (frame_buffer[i + 1 + 16*h] == 0)
/*void reconstruct_msg(){
  int i = 0;
  int j = 0;
  do{
    if(decoder(i)){
      msg |= (1<<j);
      i++;
      i++;
      j++;
    }else{
      msg &= ~(1<<j);
      i++;
      i++;
      j++;
    }
  }while (j < 8);
  h++;
  Serial.print((char)msg);
  msg = 0x00;
}*/
/*void reconstruct_msg(){
  int index = 0;
  int j = 0;
  do{
    if((frame_buffer[index + 16*h] == 1)){
      index++;
      if((frame_buffer[index + 16*h] == 0)){
        msg |= (1<<j);
        index++;
      }else{
        Serial.print("erro");
      }
      j++;
    }else if((frame_buffer[index + 16*h] == 0)){
      index++;
      if(frame_buffer[index + 16*h] == 1){
        msg &= ~(1<<j);
        index++;
      }else{
        Serial.print("erro 2");
      }
      j++;
    }
  }while (j < 8);
  h++;
  Serial.print((char)msg);
  msg = 0x00;
}*/
void reconstruct_msg(){
  int index = 0;
  int j = 0;
  do{
    if((frame_buffer[index + 16*h] == 1)&&(frame_buffer[index + 1 + 16*h] == 0)){
      msg |= (1<<j);
      index++;
      index++;
      j++;
    }else{
      msg &= ~(1<<j);
      index++;
      index++;
      j++;
    }
  }while (j < 8);
  h++;
  Serial.print((char)msg);
  msg = 0x00;
}
