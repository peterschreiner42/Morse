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
* There's still lots of stuff to do and clean up, but I wanted to share the idea
* and the current implementation to help spur people on.
*/
#include <avr/pgmspace.h>
#include <string.h>

const int ledPin = 13;
const int inputPin = 2;

#define THRESHOLD 3
#define DELAY_TIME 10

#define NCHARS 26 + 10 + 3

char Morse_AsccisChars[NCHARS] =
    {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', ',', '?'};

char *Morse_MorseChars[NCHARS] =
    {
        ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...", "---..", "----.", ".-.-.-", "--..--", "..--.."};

#define PAUSE 0
#define DIT 1
#define DAH 2
#define DDLEN 5

char password[] = "SOS";
char receivedChars[10];
int receivedCharsCursor = 0;

int morseSequence[DDLEN];
int morseSequenceCursor = 0;

void setup()
{
  pinMode(ledPin, OUTPUT);
  pinMode(inputPin, INPUT);

  for (int i = 0; i < DDLEN; i++)
  {
    morseSequence[i] = 0;
  }

  morseSequenceCursor = 0;

  for (int i = 0; i < 10; i++)
  {
    receivedChars[i] = 0;
  }

  receivedCharsCursor = 0;

  Serial.begin(9600);
}

void printMorseSequence()
{
  Serial.print("  > Morse Sequence: ");
  for (int i = 0; i < DDLEN; i++)
  {
    Serial.print(morseSequence[i]);
    Serial.print(", ");
  }
  Serial.println();
}

boolean compareMorseSequence(char *sequence)
{
  if (morseSequence[0] == PAUSE)
    return false;

  int i;

  for (i = 0; morseSequence[i] != PAUSE && i < DDLEN; i++)
  {
    if (morseSequence[i] == DIT && sequence[i] == '-')
    {
      return false;
    }
    if (morseSequence[i] == DAH && sequence[i] == '.')
    {
      return false;
    }
  }

  if (i != strlen(sequence))
    return false;

  return true;
}

void decodeMorse()
{
  for (int i = 0; i < NCHARS; i++)
  {
    if (compareMorseSequence(Morse_MorseChars[i]))
    {
      char c = Morse_AsccisChars[i];
      emitCharacter(c);
      break;
    }
  }

  for (int i = 0; i < DDLEN; i++)
  {
    morseSequence[i] = 0;
  }

  morseSequenceCursor = 0;
}

void checkPassword()
{
  Serial.print("Check password: ");
  Serial.print(receivedChars);
  Serial.println();

  if (0 != strstr(receivedChars, password))
  {
    digitalWrite(ledPin, HIGH);
  }
}

void emitCharacter(char c)
{
  Serial.print("Got a character: c=");
  Serial.println(c);
  receivedChars[receivedCharsCursor] = c;
  receivedCharsCursor++;
  if (receivedCharsCursor >= strlen(password))
  {
    checkPassword();
  }
}

void emitMorse(int value)
{
  morseSequence[morseSequenceCursor] = value;
  morseSequenceCursor++;

  printMorseSequence();
  if (value == PAUSE)
    decodeMorse();
}

void loop()
{
  int val = digitalRead(inputPin);
  static int timeSinceLow = 0;
  static int timeInLow = 0;

  if (val == LOW)
  {
    if (timeSinceLow > 5)
    {
      if (timeSinceLow > 16)
      {
        emitMorse(DAH);
      }
      else
      {
        emitMorse(DIT);
      }
    }

    timeSinceLow = 0;
    timeInLow++;
  }
  else
  {
    timeSinceLow++;
  }

  if (timeInLow == 20)
  {
    emitMorse(PAUSE);
  }

  if (timeSinceLow > THRESHOLD)
  {
    if (timeInLow > 0)
    {
      // digitalWrite(ledPin, HIGH);
    }

    timeInLow = 0;
  }
  else
  {
    // digitalWrite(ledPin, LOW);
  }

  delay(DELAY_TIME);
}
