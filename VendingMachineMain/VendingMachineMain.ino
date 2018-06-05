/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

#include <SoftwareSerial.h>
#include <Bounce2.h>

#define COIN_IN 11
#define COIN_NULL 10

#define MOTOR_OUT 12
#define MOTOR_IN 13


#define UNDETERMINED_PHASE -1

# coin phase will listen to the coin collector and button presses
#define COIN_PHASE 0

# motor phase will talk and listen to the motor controller
#define MOTOR_PHASE 1

#define EOL 255;
#define TEST_VAL 222;
#define HANDSHAKE 233;
#define NO_DATA 999;



SoftwareSerial coinSerial = SoftwareSerial(COIN_IN, COIN_NULL);
SoftwareSerial motorSerial = SoftwareSerial(MOTOR_IN,MOTOR_OUT);

int sendData = NO_DATA;

int length = 0;
char response[20];

int commPhase = UNDETERMINED_PHASE;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:

  pinMode(COIN_IN, INPUT);
  pinMode(MOTOR_IN, INPUT);
  pinMode(MOTOR_OUT, OUTPUT);

  Serial.begin(9600);
  Serial.println("fish thing ready ++");
  coinSerial.begin(4800);
  motorSerial.begin(4800);

  //setCoinPhase();
}




// the loop routine runs over and over again forever:
void loop() {
  testControllersConnection();
  sendData = NO_DATA;
  //readButtons();
  readCoinMachine();
  readMotorController();
}



void readCoinMachine(){
  if ( coinPhase() ) {
    if (coinSerial.available()) {
      int c=coinSerial.read();
      if ( c != EOL ) {
        sendData = c;
        debugChar(c);
        setMotorPhase();
      }
    }
  }
}


void readMotorController(){
  if ( motorPhase() ) {
    if ( sendData != NO_DATA ) {
      motorSerial.print(sendData);
    }

    if (motorSerial.available()) {
      int m = motorSerial.read();
      if ( m != EOL ) {
        debugChar(m);
        setCoinPhase();
      }
    }
  }
}

void readButtons(){

}

void debugChar( char c ){
  Serial.print(c);
}


void setCoinPhase(){
  commPhase = COIN_PHASE;
  coinSerial.listen();
}

void setMotorPhase(){
  commPhase = MOTOR_PHASE;
  motorSerial.listen();
}

bool coinPhase(){
  return ( commPhase == COIN_PHASE );
}

bool motorPhase(){
  return ( commPhase == MOTOR_PHASE );
}

bool testControllersConnection(){
    motorSerial.listen();
    motorSerial.print( TEST_VAL );

    bool exit = false;
    int escape = 0;

    while ( exit != true ) {
      if (motorSerial.available()) {
        int m = motorSerial.read();
        if ( m != EOL && m == HANDSHAKE ) {
          debugChar(m);
          return true;
        }
      }
      escape++;
      if ( escape > 10000 ) {
        return false;
      }
    }
}
