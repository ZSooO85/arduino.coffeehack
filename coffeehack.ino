// cubetech coffeehack
// include serial library and define serial pins on Arduino (4 and 5 works perfectly, 1 and 2 doesnt)
#include <SoftwareSerial.h>
SoftwareSerial Serial1(4, 5); // RX TX

// define some basics
#ifndef bitRead
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))
#endif

// define some variables
byte z0, z1, z2, z3;
byte x0, x1, x2, x3, x4;
byte d0, d1, d2, d3;
byte intra = 1, inter = 7;
int espresso, doubleespresso, coffee, doublecoffee, unknown, alacarte, powder;
String hexval;
String outputStr;

// open serial communication
void setup() {
  // serial console to base
  Serial.begin(9600);
  // serial connection to coffeemaker
  Serial1.begin(9600);
  Serial.println("Starting cubetech juracoffee hacker");
}

// read from coffeemaker and decrypt
byte fromCoffeemaker(byte x0, byte x1, byte x2, byte x3) {
  bitWrite(x4, 0, bitRead(x0,2));
  bitWrite(x4, 1, bitRead(x0,5));
  bitWrite(x4, 2, bitRead(x1,2));
  bitWrite(x4, 3, bitRead(x1,5));
  bitWrite(x4, 4, bitRead(x2,2));
  bitWrite(x4, 5, bitRead(x2,5));
  bitWrite(x4, 6, bitRead(x3,2));
  bitWrite(x4, 7, bitRead(x3,5));
  return x4;
}

// encrypt and send to coffeemaker
byte toCoffeemaker(byte z) {
  z0 = 255;
  z1 = 255;
  z2 = 255;
  z3 = 255;

  bitWrite(z0, 2, bitRead(z,0));
  bitWrite(z0, 5, bitRead(z,1));
  bitWrite(z1, 2, bitRead(z,2));
  bitWrite(z1, 5, bitRead(z,3));
  bitWrite(z2, 2, bitRead(z,4));
  bitWrite(z2, 5, bitRead(z,5));
  bitWrite(z3, 2, bitRead(z,6));
  bitWrite(z3, 5, bitRead(z,7));

  delay(intra); Serial1.write(z0);
  delay(intra); Serial1.write(z1);
  delay(intra); Serial1.write(z2);
  delay(intra); Serial1.write(z3);
  delay(inter);
}

// send CR LF to coffeemaker and make a delay
void sendCrLf() {
  toCoffeemaker(0x0D); delay(inter);
  toCoffeemaker(0x0A); delay(100);
}

// execute any command to coffeemaker, for example AN:02 for machine off
void getStr(String outputStr)
{
  // send each char to coffeemaker
  for (byte a = 0; a < outputStr.length(); a++){
    toCoffeemaker(outputStr[a]); delay(inter);
  }
  // send CR LF
  sendCrLf();

  String r = "";
  
  // read bits
  while(Serial1.available()) {
    delay (intra); d0 = Serial1.read();
    delay (intra); d1 = Serial1.read();
    delay (intra); d2 = Serial1.read();
    delay (intra); d3 = Serial1.read();
    delay (inter);
    r += char(fromCoffeemaker(d0,d1,d2,d3));
  }
  
  // print result to console
  Serial.print(outputStr);
  Serial.print(":.....");
  Serial.println(r);
}

// get that integer out of memory
int getVal(String outputStr)
{
  // send each char to coffeemaker
  for (byte a = 0; a < outputStr.length(); a++){
    toCoffeemaker(outputStr[a]); delay(inter);
  }
  // send CR LF
  sendCrLf();

  String r = "";
  
  // read
  while(Serial1.available()) {
    delay (intra); d0 = Serial1.read();
    delay (intra); d1 = Serial1.read();
    delay (intra); d2 = Serial1.read();
    delay (intra); d3 = Serial1.read();
    delay (inter);
    r += char(fromCoffeemaker(d0,d1,d2,d3));
  }
  
  // read real values and convert to integer
  String hex = r.substring(3,7);
  int number = (int)strtol(hex.c_str(), NULL, 16);
  return number;
}

// checks all stored values from RE:00 to RE:FF
void checkValues() {
  // go through each
  for ( int i = 0; i <= 0xFF; i++ ) {
    String outputString = "RE:";
    if(i <= 0xF){
      outputString += "0";
    }
    outputString += String(i, HEX);
    outputString.toUpperCase();
    // get value and return to console
    Serial.print(outputString); Serial.print("  "); Serial.println(getVal(outputString));
  }
}

// loop
void loop() {
  // reading coffee values
  Serial.println("------ reading values");
  espresso = getVal("RE:00");
  doubleespresso = getVal("RE:01");
  coffee = getVal("RE:02");
  doublecoffee = getVal("RE:03");
  unknown = getVal("RE:04");
  alacarte = getVal("RE:05");
  powder = getVal("RE:06");

  // print values to console
  Serial.print("ct espresso: "); Serial.println(espresso + doubleespresso);
  Serial.print("ct coffee: "); Serial.println(coffee + doublecoffee + unknown + alacarte + powder);
  Serial.print("ct total: "); Serial.println(espresso + doubleespresso + coffee + doublecoffee + unknown + alacarte + powder);

  // get all stored values
  // checkValues();

  // delay 15 seconds
  delay(15 * 1000);

}

// TODO: trigger some magic
void trigger(String ctype)
{
  Serial.println(" -- triggering webhook for a " + ctype);
  //do something
}

