#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);
SoftwareSerial mySerial_gsm(8, 9);//SIM800A Tx & Rx is connected to Arduino #8 & #9 #else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
int OTP; int Predefined_value = 1501; int i = 0, Temp = 1;

#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 6, 5, 4}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
char entryStr[12];   // This can hold up to 4 digits
int keypadval;
int cursor1 = 0, flag = 0;
int sw1 = 13, sw2 = 12;

void setup()
{
  pinMode(sw1, OUTPUT); pinMode(sw2, OUTPUT);
  digitalWrite(sw1, LOW); digitalWrite(sw2, LOW);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);  mySerial.begin(9600);//GSM);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } 
else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
  finger.getTemplateCount();
  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  ***WELCOME***");
  lcd.setCursor(0, 1);
  lcd.print("  Voting System");
  delay(3000);
}
void loop()                     // run over and over again
{
  while (Temp)
  {
    int val = getFingerprintID();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please put your");
    lcd.setCursor(0, 1);
    lcd.print("Finger on Sensor");
    delay(50);
    if (val == 10)
    {
      flag = 1;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authorisd Finger");
      lcd.setCursor(0, 1);
      lcd.print("Sending OTP.....");
      delay(2000);
      Serial.println("Finger1 authenticated");
      Temp = 0;
      SEND_OTP_SMS();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Sent OTP Sucefuly");
      delay(2000);
    }
    if (val == 20)
    {
      flag = 2;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authorisd Finger");
      lcd.setCursor(0, 1);
      lcd.print("Sending OTP.....");
      delay(2000);
      Serial.println("finger authenticated");
      Temp = 0;
      SEND_OTP_SMS();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Sent OTP Sucefuly");
      delay(2000);
    }
    if (val == 30)
    {
      flag = 3;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Authorisd Finger");
      lcd.setCursor(0, 1);
      lcd.print("Sending OTP.....");
      delay(2000);
      Serial.println("finger authenticated");
      Temp = 0;
      SEND_OTP_SMS();
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("Sent OTP Sucefuly");
      delay(2000);
    }
  }
  while (Temp != 1)
  {
    cursor1 = 0;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter OTP:");
    OTP = GetNumber();
    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("OTP :");
    lcd.setCursor(7, 0);
    lcd.print(OTP);
    lcd.setCursor(0, 1);
    lcd.print("");
    delay(2000);
    if (OTP == Predefined_value)
    {
      //Serial.println("Succesfully verified OTP");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Succesfully...");
      lcd.setCursor(0, 1);
      lcd.print(" Verified OTP");
      Temp = 1;
      delay(2000);
    }
    else if (OTP != Predefined_value)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Invalid OTP");
      lcd.setCursor(0, 1);
      lcd.print("Exited Process");
      delay(3000);
      Temp = 1;
      flag = 0;
    }
  }
  if (flag == 1)
    user1();
  else if (flag == 2)
    user2();
  else if (flag == 3)
    user3();
  delay(50);            //don't ned to run this at full speed.
}

void user1()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome ABHAY");
  lcd.setCursor(0, 1);
  lcd.print("Cast your Vote");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:BJP2:CONG3:JDS");
  cursor1 = 0;
  keypadval = GetNumber();
  switch (keypadval) {
    case 1: digitalWrite(sw1, HIGH); digitalWrite(sw2, LOW); break;
    case 2: digitalWrite(sw1, LOW); digitalWrite(sw2, HIGH); break;
    case 3: digitalWrite(sw1, HIGH); digitalWrite(sw2, HIGH); break;
  }
  delay(1000);

  if (keypadval == 1 || keypadval == 2 || keypadval == 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Thank you! for");
    lcd.setCursor(0, 1);
    lcd.print("Casting your Vote");
    delay(2000);
  }
  if (keypadval > 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Your Selected");
    lcd.setCursor(0, 1);
    lcd.print("None of Above");
    delay(2000);
  }
  flag = 0;
  digitalWrite(sw1, LOW); digitalWrite(sw1, LOW);
}
void user2()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome Madhuri");
  lcd.setCursor(0, 1);
  lcd.print("Cast your Vote");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:BJP 2:CONG 3:JDS");
  cursor1 = 0;
  keypadval = GetNumber();
  switch (keypadval) {
    case 1: digitalWrite(sw1, HIGH); digitalWrite(sw2, LOW); break;
    case 2: digitalWrite(sw1, LOW); digitalWrite(sw2, HIGH); break;
    case 3: digitalWrite(sw1, HIGH); digitalWrite(sw2, HIGH); break;
  }
  delay(1000);

  if (keypadval == 1 || keypadval == 2 || keypadval == 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Thank you! for");
    lcd.setCursor(0, 1);
    lcd.print("Casting your Vote");
    delay(2000);
  }
  if (keypadval > 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Your Selected");
    lcd.setCursor(0, 1);
    lcd.print("None of Above");
    delay(2000);
  }
  flag = 0;
  digitalWrite(sw1, LOW); digitalWrite(sw2, LOW);
}

void user3()
{  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome MANSI");
  lcd.setCursor(0, 1);
  lcd.print("Cast your Vote");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1:BJP 2:CONG 3:JDS");
  cursor1 = 0;
  keypadval = GetNumber();
  switch (keypadval) {
    case 1: digitalWrite(sw1, HIGH); digitalWrite(sw2, LOW); break;
    case 2: digitalWrite(sw1, LOW); digitalWrite(sw2, HIGH); break;
    case 3: digitalWrite(sw1, HIGH); digitalWrite(sw2, HIGH); break;
  }
  delay(1000);

  if (keypadval == 1 || keypadval == 2 || keypadval == 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Thank you! for");
    lcd.setCursor(0, 1);
    lcd.print("Casting your Vote");
    delay(2000);
  }
  if (keypadval > 3)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Your Selected");
    lcd.setCursor(0, 1);
    lcd.print("None of Above");
    delay(2000);
  }
  flag = 0;
  digitalWrite(sw1, LOW); digitalWrite(sw2, LOW);
}

void SEND_OTP_SMS()
{
  i++;
  if ((i % 2) == 1)
    Predefined_value = (Predefined_value + 1456);
  else
    Predefined_value = (Predefined_value - 328);
  if (Predefined_value > 9999)
    Predefined_value = 0123;
  Serial.print("OTP-"); Serial.println(Predefined_value);
  mySerial.println("AT+CMGF=1");   // text mode on
  delay(1000);
  mySerial.println("AT+CMGS=\"+919448508980\"\r");
  delay(1000);
  mySerial.println("OTP-"); //text content
  delay(100);
  //mySerial.println(Predefined_value);
  delay(100);
  mySerial_gsm.write(26);
    delay(1000);
}

//void updateSerial()
//{
//  delay(500);
//  while (Serial.available())
//  {
//    mySerial_gsm.write(Serial.read());//Forward what Serial received to Software Serial Port
//  }
//  while (mySerial_gsm.available())
//  {
//    Serial.write(mySerial_gsm.read());//Forward what Software Serial received to Serial Port
//  }
//}

int GetNumber()
{
  long num = 0;
  char key = kpd.getKey();
  while (key != '#')
  {
    switch (key)
    {
      case NO_KEY:
        break;
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        lcd.setCursor(cursor1, 1);
        lcd.print(key);
        cursor1 = cursor1 + 1;

        if (cursor1 == 5)
        {
          cursor1 = 0;
        }
        //  Serial.print(key);
        num = num * 10 + (key - '0');
        //            Serial.print("num: ");
        //             Serial.println(num);
        break;
      case '*':
        num = 0;
        //lcd.clear();
        break;
    }
    key = kpd.getKey();
  }
  return num;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Didnt find match");
    delay(1000);
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
