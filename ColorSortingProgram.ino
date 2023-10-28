// ----- pin constants -----

const int rotaryEncoder = 2; // this is attach interrupt 0
const int limitSwitch = 3;
const int photo = 4;
const int motor1A = 5;
const int motor2A = 6;
const int armValve = 7;
const int suctionCup = 8;
const int enable = 9;
const int colorSensor = A0;

// ----- constants -----

const int stopMotor = 0;
const int counterClockwise = 1;
const int clockwise = 2;
const int motorSpeed = 255;

const bool motorDir1 = LOW;
const bool motorDir2 = HIGH;

// ----- changing variables -----

volatile int count = 0, mode = 0, binPos = 1, colorVal;
volatile unsigned long lastPressTime = millis();

volatile bool nowButtonState;
volatile bool lastButtonState;
volatile bool stop = LOW;



// ----- the set up section -----

void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  // outputs
  pinMode(motor1A,        OUTPUT); 
  pinMode(motor2A,        OUTPUT);
  pinMode(enable,         OUTPUT);
  pinMode(suctionCup,     OUTPUT);
  pinMode(armValve,       OUTPUT);

  // inputs
  pinMode(rotaryEncoder,  INPUT);
  pinMode(limitSwitch,    INPUT);
  pinMode(photo,          INPUT);
  pinMode(colorSensor,    INPUT);

  // start the program with the enable pin high
  analogWrite(enable, motorSpeed); 

  // this function will be used by the rotary encoder to gauge the position of the robot arm
  // using the CHANGE mode of the attachInterrupt function effectively doubles the resolution of the encoder by using the rising and falling edges of the signals.
  attachInterrupt(digitalPinToInterrupt(rotaryEncoder), encoderInterrupt, CHANGE); 
}



// ----- these are functions used in the loop -----

int getBinPos(int colorVal){
  if(colorVal <= 590) 
    return 2; // the color is white (around 615 - 630)
  else if( colorVal > 585 && colorVal < 700) 
    return 3; // the color is red (around 650)
  else if(colorVal > 700)
    return 1; // the color is blue
}

bool sortPuck(int bin, int count){ 
  // This is a function that checks to see if we are where we need to be or not. 
  // Once we are where we need to be, we will STOP the bot and wait a second.
  if(bin == 1 && count >= 3)
    return HIGH;
  else if(bin == 2 && count >= 5)
    return HIGH;
  else if(bin == 3 && count >= 7)
    return HIGH;
  else
    return LOW;
}

void printData(){ // This function prints the current encoder count, mode, and bin location. This is for debugging purposes.
  Serial.print("  count: ");
  Serial.print(count);
  Serial.print("  mode: ");
  Serial.print(mode);
  Serial.print("  bin: ");
  Serial.println(binPos);
}

void motorDirection(int dir){ // this function controls the motion of the robot arm. 
  if(dir == 0){ // stop by putting both values low
    digitalWrite(motor1A, LOW);
    digitalWrite(motor2A, LOW);
  }
  else if(dir == 1){ // counter clockwise
    digitalWrite(motor1A, motorDir1);
    digitalWrite(motor2A, motorDir2);
  }
  else if(dir == 2){ // clockwise
    digitalWrite(motor1A, !motorDir1);
    digitalWrite(motor2A, !motorDir2);
  }
}

void encoderInterrupt(){
  nowButtonState = digitalRead(rotaryEncoder); // this sets the button state. this is the rotary encoder
  if(nowButtonState == !lastButtonState && millis() - lastPressTime > 50){ // if the button state is high and time between clicks (current time - last known press time) is less than 50
    count++; // increase button press count. this is our counter for the arm rotations
    lastPressTime = millis(); // storing the current time of the global clock for for the next if statement calculation as shown above
  }
  lastButtonState = nowButtonState; // reset the now button state
}



// ----- the loop routine runs over and over again forever:

void loop() {
  stop = digitalRead(limitSwitch); // this is the variable that tells the motor to stop rotating once it reaches the limit switch

  if(mode == 0){ // this is the return to home mode and default mode. This mode waits for pucks to sort based on the phototransitor value. 
    if(stop == LOW){ // return to home if stop is low 
      motorDirection(counterClockwise); // counter clock wise
    }
    else if(stop == HIGH && digitalRead(photo) == LOW){ // if stop is high and there is no puck stop but don't change modes
      analogWrite(enable, 0); // this sets the enable pin on the H-bridge low to stop the motor
    }
    else{
      analogWrite(enable, motorSpeed); // sets the motor speed for the rest of operation
      motorDirection(stopMotor); // stop motor and pick up the puck
      delay(100);
      digitalWrite(armValve, HIGH); // lower the arm
      delay(250);
      digitalWrite(suctionCup, HIGH); // generate the vacuum against the puck
      delay(500);
      digitalWrite(armValve, LOW); // raise the arm
      delay(250);
      mode = 1; // continue to next mode
      count = 0; // reset the positional count
    }
  }
  else if(mode == 1){ // this goes to the color sensor
    if(count < 6){ // while there is fewer than 6 encoder clicks, keep turning clockwise
      motorDirection(clockwise); // clockwise
    }
    else{ // if greater than or equal to 4, stop the robot
      motorDirection(stopMotor); // stop the arm's rotation now that it is above the color sensor
      digitalWrite(armValve, HIGH); // lower the arm
      delay(1500); // delay to obtain an accurate reading from the sensor
      colorVal = analogRead(A0); // read the color value given by the analog color sensor
      binPos = getBinPos(colorVal); // call the getBinPos() function to assign the bin position
      delay(500);
      digitalWrite(armValve, LOW); // raise the arm
      //printData(); // serial monitor print function found above for debugging
      delay(250);
      mode = 2; // continue to next mode
      count = 0; // reset the positional count
    }
  }
  else if(mode == 2){
    if(sortPuck(binPos, count) == LOW){ // rotate until the arm is at the correct location
      motorDirection(clockwise); 
    }
    else if(sortPuck(binPos, count) == HIGH){ // arm is at the correct bin location
      motorDirection(stopMotor);// stop rotation
      // printData(); // serial monitor print function found above for debugging
      digitalWrite(armValve, HIGH); // lower the arm
      digitalWrite(suctionCup, LOW); // release the vacuum to release the puck
      delay(500); 
      digitalWrite(armValve, LOW); // raise the arm
      mode = 0; // continue to the default mode to start homing
      count = 0; // reset the positional count
    }
  }
}
