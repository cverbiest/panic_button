#include <PS2Keyboard.h>
#include <Bounce.h>

/*
Author : Carl Verbiest
Created : Jul 9, 2012
Purpose : Teensy 2.0 code for a windows lockdown button
*/

const int redPin =  15;
const int greenPin =  14;
const int bluePin =  12;
const int internalLED = 11;
const int buttonPin = 7;
const int button2Pin = 6;
const int MaxBufferSize = 512;

const int DataPin = 8;
const int IRQpin =  5;

PS2Keyboard keyboard;

Bounce pushbutton = Bounce(button2Pin, 10);  // 10 ms debounce

int warnLevel = 0;
int beenUp = 0;
int SerialOn = 0;
int downCount = 0;
int BufferSize = 0;
char lastinfo[100];
char SerialBuffer[512];


// The setup() method runs once, when the sketch starts

void setup()   {                
  delay(1000);
  // initialize the digitals pin as an outputs
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(internalLED, OUTPUT);
  
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  
  Serial.begin(38400);
  SerialOn = 1;

  keyboard.begin(DataPin, IRQpin, PS2Keymap_belatin1);
  
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

void GetKeyScanCode()
{
  char info[100];
  char translated_char[3];
  uint8_t scancode = keyboard.get_raw_scan_code(translated_char);
  if (scancode) {
    uint8_t row = scancode / 8 + 1;
    uint8_t pos = scancode % 8 + 1;
    sprintf(info, "scan code %i, row %i pos %i, %c %c %c", scancode, row, pos, translated_char[0], translated_char[1], translated_char[2] );
    Serial.println(info);

  }
}
void CheckKeyboard()
{
  if (keyboard.available()) {
    
    // read the next key
    char c = keyboard.read();
    
    // check for some of the special keys
    if (c == PS2_ENTER) {
      Serial.println();
    } else if (c == PS2_TAB) {
      Serial.print("[Tab]");
    } else if (c == PS2_ESC) {
      Serial.print("[ESC]");
    } else if (c == PS2_PAGEDOWN) {
      Serial.print("[PgDn]");
    } else if (c == PS2_PAGEUP) {
      Serial.print("[PgUp]");
    } else if (c == PS2_LEFTARROW) {
      Serial.print("[Left]");
    } else if (c == PS2_RIGHTARROW) {
      Serial.print("[Right]");
    } else if (c == PS2_UPARROW) {
      Serial.print("[Up]");
    } else if (c == PS2_DOWNARROW) {
      Serial.print("[Down]");
    } else if (c == PS2_DELETE) {
      Serial.print("[Del]");
    } else {
      
      // otherwise, just print all normal characters
      Serial.print(c);
    }
  }
}

void loop()                     
{
  char info[100];

  // Process serial input 
  unsigned char bytecount = 0;
  unsigned char incomingByte = 0;
  while (Serial.available() && bytecount < 64) {
    incomingByte = Serial.read();
    if (incomingByte == 13) {
      SerialBuffer[BufferSize++] = 0;
      sprintf(info, "Received:%s", SerialBuffer);
      feedback(info);
      BufferSize = 0;
    } else {
      SerialBuffer[BufferSize++] = incomingByte;
      if (incomingByte < 33 or incomingByte > 127) {
        sprintf(info, "Detected chr(%i), %i in buffer", incomingByte, BufferSize);
        feedback(info);
      }
      
    }
    bytecount++;
  }

  GetKeyScanCode();
  // CheckKeyboard();
  
  if (pushbutton.update()) {
    if (pushbutton.fallingEdge()) {
    }
    feedback("pushbutton.fallingEdge");
    Keyboard.print(SerialBuffer);
  }
/*  
  // Pin HIGH = button not pressed
  if (digitalRead(button2Pin) == HIGH) {
    digitalWrite(internalLED, LOW);
    // feedback("Button II not pressed...");
  } else {
    digitalWrite(internalLED, HIGH);
    feedback("Button II pressed");
    Keyboard.print(SerialBuffer);
  }
*/

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
