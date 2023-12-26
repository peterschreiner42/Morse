// morse-decode-encode
// by Oompah
// March 3 2023
// credit to "Programming Arduino" by Simon Monk, (c)2016
// for some of the code lines
//
// Either Key in a Morse code sequence using the button to decode,
// or click in the field in the Serial Monitor, and
// type letters followed by Enter to encode into Morse code.
// The LED while you key in the sequence.
// This sketch decodes sequences with letters or numbers
// and prints them to the serial monitor.
// A beep sounds and "?" prints for unrecognized sequences.
//
// To learn Morse Code, type a word in the Serial Monitor,
// Then try keying in the same word as you heard it.
// Tips:
//   Keep dots short, and dashes long.
//   Leave a short space after letters.
//   Leave a longer space after words.
//   Try to make nice clean presses and releases.
//
// Circuit:
// Piezo buzzer or speaker connects to pin 2 and ground
// Button connects between pin 8 and ground
// LED (+) to pin 13, (-) 220 Ohm resistor to ground
//
int tonePin = 8;  //D8
int toneFreq = 1000;
int ledPin = 13;    //D13
int buttonPin = 2;  //D2
int debounceDelay = 30;

int dotLength = 240;
// dotLength = basic unit of speed in milliseconds
// 240 gives 5 words per minute (WPM) speed.
// WPM = 1200/dotLength.
// For other speeds, use dotLength = 1200/(WPM)
//
// Other lengths are computed from dot length
int dotSpace = dotLength;
int dashLength = dotLength * 3;
int letterSpace = dotLength * 3;
float wpm = 1200. / dotLength;

int resetLength = 2000;

int t1, t2, onTime, gap;
bool newLetter, newWord, letterFound;
// int lineLength = 0;
// int maxLineLength = 20;

char* letters[] = {
  ".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..",    // A-I
  ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.",  // J-R
  "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--.."          // S-Z
};

char* numbers[] = {
  "-----", ".----", "..---", "...--", "....-",  //0-4
  ".....", "-....", "--...", "---..", "----."   //5-9
};

String dashSeq = "";
String GesResult = "";
char keyLetter, ch;
int i, index;

void setup() {
  delay(500);
  pinMode(ledPin, OUTPUT);
  pinMode(tonePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);

  WriteStartScreen();

  //if false, do NOT check for end of letter gap
  newLetter = false;
  //if false, do NOT check for end of word gap
  newWord = false;
}

void loop() {

  if (digitalRead(buttonPin) == LOW)  //button is pressed
  {
    newLetter = true;
    newWord = true;
    t1 = millis();               //time at button press
    digitalWrite(ledPin, HIGH);  //turn on LED and tone
    delay(debounceDelay);

    while (digitalRead(buttonPin) == LOW)  // wait for button release
    {
      delay(debounceDelay);
    }

    delay(debounceDelay);

    t2 = millis();              //time at button release
    onTime = t2 - t1;           //length of dot or dash keyed in
    digitalWrite(ledPin, LOW);  //torn off LED and tone

    //check if dot or dash
    if (onTime <= dotLength * 1.5)  //allow for 50% longer
    {
      dashSeq = dashSeq + ".";
    }  //build dot/dash sequence
    else if (onTime >= resetLength) {
      DoReset();
    } else {
      dashSeq = dashSeq + "-";
    }
  }  //end button press section

  // look for a gap >= letterSpace to signal end letter
  // end of letter when gap >= letterSpace
  gap = millis() - t2;

  if (newLetter == true && gap >= letterSpace) {
    //check through letter sequences to find matching dash sequence

    letterFound = false;
    keyLetter = 63;  //char 63 is "?"

    for (i = 0; i <= 25; i++) {
      if (dashSeq == letters[i]) {
        keyLetter = i + 65;
        GesResult += keyLetter;
        letterFound = true;
        break;  //don't keep checking if letter found
      }
    }

    Serial.print(keyLetter);

    if (letterFound == false)  //buzz for unknown key sequence
    {
      Serial.print("unbekannte key sequenz");
    }

    newLetter = false;  //reset
    dashSeq = "";
  }

  // Der Schlüsselbuchstabe wurde identifiziert und gedruckt
  // Wenn die Lücke >= Wortabstand ist, fügen Sie einen Zwischenraum zwischen den Wörtern ein
  // Verlängern Sie den Wortabstand um 50%, um Variationen zu ermöglichen

  int index = GesResult.indexOf("SOS");
  if (index != -1) {
    Serial.println("");
    Serial.println("SOS Gefunden");
    GesResult = "";

    // ************************************************************************
    // ************************************************************************
    // Hier die Aktion einbinden, die ausgeführt werden soll, wenn die SOS per Taster eingegeben wurde
    // ************************************************************************
    // ************************************************************************
  }
}

//*************************************************************

void DoReset() {
  newLetter = false;
  newWord = false;
  GesResult = "";
  Serial.println("Reset");
  delay(100);
  WriteStartScreen();
  return;
}

// Schreibt die Startmeldungen auf den Serial Monitor
void WriteStartScreen() {
  Serial.println();
  Serial.println("-------------------------------");
  Serial.println("SOS Recognition");
  Serial.print("Speed=");
  Serial.print(wpm);
  Serial.print("wpm, ");
  Serial.print("dot=");
  Serial.print(dotLength);
  Serial.println("ms");

  Serial.println("For reset, hold the button for");
  Serial.print(resetLength);
  Serial.println(" milliseconds");
  Serial.println("-------------------------------");
  Serial.println("Ready");
}

void flashSequence(char* sequence) {
  int i = 0;
  while (sequence[i] == '.' || sequence[i] == '-') {
    flashDotOrDash(sequence[i]);
    i++;
  }
}

void flashDotOrDash(char dotOrDash) {
  digitalWrite(ledPin, HIGH);
  tone(tonePin, toneFreq);
  if (dotOrDash == '.') {
    delay(dotLength);
  } else {
    delay(dashLength);
  }

  digitalWrite(ledPin, LOW);
  noTone(tonePin);
  delay(dotLength);
}

//--- end of sketch ---
