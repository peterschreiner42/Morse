

/*
* Morse Code Decoder and door opener
*
* Josh Myer <josh@joshisanerd.com>
*
* 20090103 rev 0 -- ugly and unfortunate, but mostly functional (my first arduino code)
*
* Hook a debounced switch up to digital pin 2, like you would for the Button demo.
*
* This code reads morse code from digital02, turning it into ASCII characters.
* If you key in "SOS" (... --- ...), it will turn on the LED on digital13.
*
* The intended application is to let me key in a password at my apartment's
* front gate and have it automatically let me into the building, instead of
* fumbling around for keys.
*
* There's still lots of stuff to do and clean up, but I wanted to share the idea
* and the current implementation to help spur people on.
*/

#include <avr/pgmspace.h>
#include <string.h>

int ledPin = 13;                // choose the pin for the LED
int inputPin = 2;               // choose the input pin (for a pushbutton)
int val = 0;                    // variable for reading the pin status

#define THRESHOLD 3
#define DELAY_TIME 10 // ms

int n_since_zero = 0;
int n_in_zero = 0;

#define NCHARS 26+10+3 // 39

char morse_chars[NCHARS]  = {
 'A',
 'B',
 'C',
 'D',
 'E',
 'F',
 'G',
 'H',
 'I',
 'J',
 'K',
 'L',
 'M',
 'N',
 'O',
 'P',
 'Q',
 'R',
 'S',
 'T',
 'U',
 'V',
 'W',
 'X',
 'Y',
 'Z',
 '0',
 '1',
 '2',
 '3',
 '4',
 '5',
 '6',
 '7',
 '8',
 '9',
 '.',
 ',',
 '?',

};

char* morse_strings[NCHARS] = {
 ".-",
 "-...",
 "-.-.",
 "-..",
 ".",
 "..-.",
 "--.",
 "....",
 "..",
 ".---",
 "-.-",
 ".-..",
 "--",
 "-.",
 "---",
 ".--.",
 "--.-",
 ".-.",
 "...",
 "-",
 "..-",
 "...-",
 ".--",
 "-..-",
 "-.--",
 "--..",
 "-----",
 ".----",
 "..---",
 "...--",
 "....-",
 ".....",
 "-....",
 "--...",
 "---..",
 "----.",
 ".-.-.-",
 "--..--",
 "..--..",
};



#define PAUSE 0
#define DIT 1
#define DAH 2
#define DDLEN 5

char passwd[] = "SOS";

char chars_rx[10];
int char_cursor= 0;

int ditsdahs[DDLEN];
int dd_cursor = 0;

void setup() {
 pinMode(ledPin, OUTPUT);      // declare LED as output
 pinMode(inputPin, INPUT);     // declare pushbutton as input

 for (int i = 0; i < DDLEN; i++) {
   ditsdahs[i] = 0;
 }
 dd_cursor = 0;


 for (int i = 0; i < 10; i++) {
   chars_rx[i] = 0;
 }
 char_cursor = 0;


 Serial.begin(9600);
}

void dd_print() {
 Serial.print("  > DD BUF: ");
 for(int i = 0; i < DDLEN; i++) {
   Serial.print(ditsdahs[i]);
   Serial.print(", ");
 }
 Serial.println();
}

boolean dd_eq(char*buf) {
 if(ditsdahs[0] == PAUSE) return false;

 int i;

 for(i = 0; ditsdahs[i] != PAUSE && i < DDLEN; i++) {
   if (ditsdahs[i] == DIT && buf[i] == '-') { 
     return false; 
   }
   if (ditsdahs[i] == DAH && buf[i] == '.') { 
     return false; 
   }
 }

 /*
 Serial.print("Got to the end of ");
  Serial.print(buf);
  Serial.print("; checking strlen=");
  Serial.print(strlen(buf));
  Serial.print(" == i=");
  Serial.println(i);
  */
 if (i != strlen(buf)) return false;

 return true;
}

void dd_decode() {
 for(int i = 0; i < NCHARS; i++) {
   /*
   Serial.print("dd_decode: check i=");
    Serial.print(i);
    Serial.print(" , string=\"");
    Serial.print(morse_strings[i]);
    Serial.print("\", c=");
    Serial.print(morse_chars[i]);
    Serial.println();
    */
   if (dd_eq(morse_strings[i])) {

     char c = morse_chars[i];

     char_emit(c);


     break;
   }
 }

 for (int i = 0; i < DDLEN; i++) {
   ditsdahs[i] = 0;
 }
 dd_cursor = 0;
}

void check_passwd() {

 Serial.print("Check password: ");
 Serial.print(chars_rx);
 Serial.println();

 if (0 != strstr(chars_rx, passwd)) {
   digitalWrite(ledPin, HIGH);
 }

 /*
 for (int i = 0; i < 10; i++) {
  chars_rx[i] = 0;
  }
  char_cursor = 0;
  */
}

void char_emit(char c) {
 Serial.print("Got a char: c=");

 Serial.println(c);


 chars_rx[char_cursor] = c;
 char_cursor++;

 if (char_cursor >= strlen(passwd)) {
   check_passwd();
 }
}

void dd_emit(int v) {
 ditsdahs[dd_cursor] = v;
 dd_cursor++;

 dd_print();

 if (v == PAUSE)
   dd_decode();
}

void dit()   { 
 dd_emit(DIT); 
 Serial.println("DIT"); 
}
void dah()   { 
 dd_emit(DAH); 
 Serial.println("DAH"); 
}
void pause() { 
 dd_emit(PAUSE); 
 Serial.println("PAUSE"); 
}

void loop(){
 val = digitalRead(inputPin);  // read input value

 if (val == LOW) {
   if (n_since_zero > 5) {
     /*  Serial.print("CROSS LOW AFTER ");
      Serial.println(n_since_zero);
      */
     if (n_since_zero > 16) { 
       dah(); 
     } 
     else {
       dit();
     }


   }

   n_since_zero = 0;
   n_in_zero++;
 } 
 else {
   n_since_zero++;
 }

 if (n_in_zero == 20) {
   pause();
 }

 if (n_since_zero > THRESHOLD) {
   if (n_in_zero > 0) {
     // Serial.print("CROSS HIGH AFTER ");
     //Serial.print(" > ");
     // Serial.println(n_in_zero);

     //      Serial.println("**************************");
   }

   n_in_zero = 0;
   // digitalWrite(ledPin, HIGH);
 } 
 else {
   // digitalWrite(ledPin, LOW);
 }

 delay(DELAY_TIME);
}
