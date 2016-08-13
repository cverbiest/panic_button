/*
Author : Carl Verbiest
Created : Jul 9, 2012
Purpose : Teensy 2.0 code for a windows lockdown button
*/

int redPin =  15;
int greenPin =  14;
int bluePin =  12;
int buttonPin = 7;
int warnLevel = 0;
int beenUp = 0;
int SerialOn = 0;
int downCount = 0;
char lastinfo[100];

// The setup() method runs once, when the sketch starts

void setup()   {                
  // initialize the digitals pin as an outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  Serial.begin(38400);
  SerialOn = 1;
  pinMode(buttonPin, INPUT);
  SetLedRGB(LOW,HIGH,LOW);
}

void SetLedRGB(int red, int green, int blue) {
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);
  digitalWrite(bluePin, blue);
}

void feedback(char txt[]) {
  if (SerialOn) { if (!strcmp(lastinfo,txt)) Serial.println(txt); strcpy(txt, lastinfo); }
}

void LockPC() {
   // Press GUI-D
    Keyboard.set_modifier(MODIFIERKEY_GUI);
    Keyboard.send_now();
    Keyboard.set_key1(KEY_D);
    Keyboard.send_now();

    // release all the keys at the same instant
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();  
    feedback("GUI-D sent, pause");
    delay(100);

    Keyboard.set_modifier(MODIFIERKEY_GUI);
    Keyboard.send_now();
    Keyboard.set_key1(KEY_L);
    Keyboard.send_now();

    // release all the keys at the same instant
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();  
    Serial.println("GUI-L sent");

    SetLedRGB(HIGH,LOW,HIGH);
    delay(1000);
    SetLedRGB(LOW,LOW,HIGH);
    delay(1000);
    SetLedRGB(LOW,HIGH,LOW);
}


void loop()                     
{
  char info[100];


  if (digitalRead(buttonPin) == HIGH) {
    beenUp = 1;
    downCount = 0;
    /* Serial.println("Button is not pressed...");*/
  } else {
    downCount++;
    if (beenUp) {
      beenUp = 0;
      Serial.println("Button pressed!!!");
      warnLevel += 500;
      if (warnLevel > 500) {
        LockPC();
        warnLevel = 0;
      }
    }
    downCount++;
    if (downCount > 200) {
      feedback("Turn off LED");
      warnLevel = 0;
      SetLedRGB(LOW,LOW,LOW);
    } 
  }
  sprintf(info, "Warning level is %i, downCount %i", warnLevel, downCount);
  feedback(info);
  if (warnLevel > 0) {
    SetLedRGB(HIGH,LOW,LOW);
    warnLevel--;
    if (warnLevel == 0) SetLedRGB(LOW,HIGH,LOW);
  }
  delay(10);
}
