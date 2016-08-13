void setup()   {                
  Serial.begin(38400);
  pinMode(7, INPUT);
}

void loop()                     
{
  if (digitalRead(7) == HIGH) {
    /* Serial.println("Button is not pressed...");*/
  } else {
    Serial.println("Button pressed!!!");
    Keyboard.set_modifier(MODIFIERKEY_GUI);
    Keyboard.send_now();
    Keyboard.set_key1(KEY_D);
    Keyboard.send_now();

    // release all the keys at the same instant
    Keyboard.set_modifier(0);
    Keyboard.set_key1(0);
    Keyboard.send_now();  
    Serial.println("GUI-D sent, pause");
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
  }
  delay(100);
}
