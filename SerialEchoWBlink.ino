#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>



unsigned int reload = 0xF425; // OCR1A = (cpu freq / (prescaler * interrupt freq)) - 1;
                              // 1Hz, a 1 second timer interrupt.
                              // cpu-freq = 16mHz, prescaler = 256 (see TCCR1B), desired interrupt freq = 1
volatile uint8_t ledstate = LOW;
const int bufsize = 256;
char data[bufsize];
int data_pos = 0;

ISR(TIMER1_COMPA_vect)
{
  digitalWrite(LED_BUILTIN, ledstate);
  ledstate = !ledstate;
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  cli(); // disable interrupts
  TCCR1A = 0; // start with blank settings
  TCCR1B = 0; // blank settings
  OCR1A = reload; // timer comparison value for interrupt trigger.
  TCCR1B = (1<<WGM12) | (1<<CS12); // Waveform gen mode: CTC ; CS12: clock/256 (CTC = clear timer on compare)
  TIMSK1 = (1<<OCIE1A); // enable the timer
  sei(); // enable interrupts.

  Serial.begin(9600);
  Serial.println();
  Serial.println("Something SOMETHING: ");
}

// echo text entered via serial, then echo a toUpper()'d buffer with some other
// modifications.
void loop() {
  char incoming = 0;
  // read a character
  if(Serial.available() > 0)
  {
    incoming = (char)Serial.read();
    if(incoming != 13 && incoming != 13)
    {
      Serial.print(incoming);
      if(data_pos < bufsize)
      {
        data[data_pos] = incoming;
        data_pos++;
      }
    }
    else
    {
      Serial.println();

      for(int i = 0; i < data_pos; i++)
      {
        char c = data[i];
        if(c >= 'a' && c <= 'z')
        {
          c = c - 'a' + 'A';
        }
        else if(c == '?')
        {
          c = '!';
        }
        Serial.print(c);
      }

      data_pos = 0;
      Serial.println();
    }
  }
}
