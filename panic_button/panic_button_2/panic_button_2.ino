/*
Author : Carl Verbiest
Created : Jul 9, 2012
Purpose : Teensy 2.0 code for a windows lockdown button
*/

int redPin =  15;
int greenPin =  14;
int bluePin =  12;
int internalLED = 11;
int buttonPin = 7;
int button2Pin = 6;

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
  pinMode(internalLED, OUTPUT);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  
  Serial.begin(38400);
  SerialOn = 1;
  
  SetLedRGB(LOW,HIGH,LOW);
}

void SetLedRGB(int red, int green, int blue) {
  digitalWrite(redPin, red);
  digitalWrite(greenPin, green);
  digitalWrite(bluePin, blue);
}

/*
send feedback over serial interface, 
*/
void feedback(char txt[]) {
  if (SerialOn) { if (strcmp(lastinfo,txt)) Serial.println(txt); strcpy(lastinfo, txt); }
}

void LockPC() {
    feedback("Locking PC");
    exit;
    
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

    // Press GUI-L
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

  // Pin HIGH = button not pressed
  if (digitalRead(button2Pin) == HIGH) {
    digitalWrite(internalLED, LOW);
    // feedback("Button II not pressed...");
  } else {
    digitalWrite(internalLED, HIGH);
    feedback("Button II pressed");
  }
  
  if (digitalRead(buttonPin) == HIGH) {
    beenUp = 1;
    downCount = 0;
    /* feedback("Button is not pressed...");*/
  } else {
    downCount++;
    if (beenUp) {
      beenUp = 0;
      feedback("Button pressed!!!");
      warnLevel += 500;
      if (warnLevel > 500) {
        LockPC();
        warnLevel = 0;
      } else {
        feedback("Arm system");
      }
    }
    // Detect long button press
    downCount++;
    if (downCount > 200) {
      feedback("Disarm system");
      warnLevel = 0;
      feedback("Turn off LED");
      SetLedRGB(LOW,LOW,LOW);
    } 
  }
//  sprintf(info, "Warning level is %i, downCount %i", warnLevel, downCount);
//  feedback(info);
  if (warnLevel > 0) {
    SetLedRGB(HIGH,LOW,LOW);
    warnLevel--;
    if (warnLevel == 0) { 
      feedback("Disarm system");
      SetLedRGB(LOW,HIGH,LOW); 
    }
  }
  delay(10);
}
