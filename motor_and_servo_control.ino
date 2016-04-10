
#include <Servo.h> 
#include "Timer.h"
 
 
Timer t;
Servo myservo;  // create servo object to control a servo
const int echoPin = 12; // ultrasonic receive=echo pin
const int triggerPin = 13; // ultrasonic send=trigger pin

//motor pin setup
const int MotorRight1 = 7;
const int MotorRight2 = 8;
const int MotorLeft1 = 9;
const int MotorLeft2 = 10;
const int MotorRightPWM = 6;
const int MotorLeftPWM = 11;
int iSpeed = 255; //speed, range 0 to 255
int slowSpeed = 150;
const int Fgo = 8; // go straight
const int Rgo = 6; // turn right
const int Lgo = 4; // turn left
const int Bgo = 2; // go back
const int rightDir = 5;
const int leftDir = 6;
const int forwardDir = 7;
bool lastTurn = 0;

void setup()
{
   //Ultra sonic
  pinMode(echoPin, INPUT);
  pinMode(triggerPin, OUTPUT);
  //motor connections
  pinMode(MotorRight1, OUTPUT);  //
  pinMode(MotorRight2, OUTPUT);  //
  pinMode(MotorLeft1,  OUTPUT);  //
  pinMode(MotorLeft2,  OUTPUT);  //
  pinMode(MotorRightPWM, OUTPUT); //enable for right side motor
  pinMode(MotorLeftPWM, OUTPUT); //enable for right side motor

   myservo.attach(2);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  //int tickEvent = t.every(3000, sweep);
  //int tickEvent2 = t.every(200, getDistance);
  //int afterEvent = t.every(1000, doAfter);
 
  } 

//****************************  MAIN LOOP  ***********************************
void loop()
{
  //t.update();
  sweep();
}
 
 
void sweep()
{

  int avgRight = 0;
  int avgLeft = 0;
  int pos = 80;
  int recent = 0;
  
    //right to middle-right sweep
  for (pos = 80; pos <= 110; pos += 5) { // goes from 80 degrees to 130 degrees                                     // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);
    recent = getDistance();
    if(recent <= 45)  //check safe distance
        escape(rightDir);
    avgRight += recent;
  }
  
  avgRight = avgRight/6;
  Serial.print("Average distance to right: ");
  Serial.print(avgRight);
  Serial.print("\n");
  
  //middle-right to middle-left sweep
  for (pos = 110; pos <= 150; pos += 5) { // goes from 130 degrees to 180 degrees
                                         // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);
    if(getDistance() <= 45)  //check safe distance
        escape(forwardDir);
  }

  
  //middle-left to left sweep
  for (pos = 150; pos <= 180; pos += 5) { // goes from 180 degrees to 130 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);
    recent = getDistance();
    if(recent <= 45)  //check safe distance
        escape(leftDir);
    avgLeft += recent;                       // waits 15ms for the servo to reach the position
}
 avgLeft = avgLeft/6;
  Serial.print("Average distance to Left: ");
  Serial.print(avgLeft);
  Serial.print("\n");
  //put compare function here
    compare(avgLeft,avgRight);
 
 //This resets distances for next sweep's compare function 
  avgRight = 0;
  avgLeft = 0;

//left to middle-left sweep
for (pos = 180; pos >= 150; pos -= 5) { // goes from 130 degrees to 80 degrees                                     // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);       // waits 15ms for the servo to reach the position
    recent = getDistance();
    if(recent <= 45)  //check safe distance
        escape(leftDir);    //escape if distance not safe
    avgLeft += recent;           
}
 avgLeft = avgLeft/6;
  Serial.print("Average distance to Left: ");
  Serial.print(avgLeft);
  Serial.print("\n");

  //middle-left to middle-right sweep
  for (pos = 150; pos >= 110; pos -= 5) { // goes from 130 degrees to 180 degrees
                                         // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);
    if(getDistance() <= 45)  //check safe distance
        escape(forwardDir);
  }

    //right to middle-right sweep
  for (pos = 110; pos >= 80; pos -= 5) { // goes from 80 degrees to 130 degrees
                                         // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(30);
    recent = getDistance();
    if(recent <= 45)  //check safe distance
        escape(rightDir);    //escape if distance not safe
    avgRight += recent;
  }
  
  avgRight = avgRight/6;
  Serial.print("Average distance to right: ");
  Serial.print(avgRight);
  Serial.print("\n");
  //////////put compare function here
  compare(avgLeft,avgRight);

}
//**********************  END MAIN LOOP *****************************************
 
 
 
 
//************Ultrasonic distance calculator*************************************
//detect distance for given angles and print char + direction
float getDistance() {
  digitalWrite(triggerPin, LOW); // ultrasonic echo low level in 2us
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH); // ultrasonic echo high level in 10us, at least 10us
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW); // ultgrasonic echo low level
  float distance = pulseIn(echoPin, HIGH); // read time
  distance = distance / 5.8 / 10; // turn time to distance 
  //Serial.print(" distance: "); // 
  //Serial.print(int(distance)); //  output distance (mm)
  //Serial.print("\n");
  return distance;
}

//*****************Absolute value function**********************************
int abso(int value)
{
  if (value < 0)
      return -value;
  else
      return value;
}

//*****************Escape function**********************************
void escape(int direction) {
  
  while(getDistance() <= 50){ //runs through loop until clearance greater than 50 cm
  Serial.print("Turning to safe distance \n"); //serial prints 

  while(getDistance() < 20){ // runs backwards until 20cm of clearance
  back(5); // backs up for 5 msec
  }
  // if clearance is greater than 20cm then it turns away from the direction sensor is facing
  if (direction == rightDir)
    turnL(5);
  else if (direction == leftDir)
    turnR(5);
   // if sensor is facing forward then it alternates turning direction everytime it sees a new object
  else{
    if(lastTurn) // checks boolean value to alternate direction when sensor facing forward
      turnL(5);
    else
      turnR(5); 
  }
  }
  Serial.print("safe to proceed \n"); // prints when escape loop finishes
  //continue forward
  advance(0); // robot moves forward
  lastTurn = !lastTurn; //alternates bool value for next turn direction
}

//**************Compare Funtion*********************************
void compare(int left, int right){
  int diff = left - right;
  if(abso(diff) <= 20){
     //continue forward
     advance(0);
     Serial.print("Continue forward \n");
  }
  else {
   if(left > right){
      //adjust left
      turnL(5);
      slightL(0);
      Serial.print("adjust left \n");
   }
   else{
      //adjust right
      turnR(5);
      slightR(0);
     Serial.print("adjust right \n"); 
   }
  }
}


//**************Movement functions******************************
void advance(int d) { //go straight
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, HIGH);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, HIGH);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);
  delay(d * 10);
}
void slightL(int d) {
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, HIGH);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, HIGH);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, 100);
  delay(d * 10);
}
void slightR(int d) {
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, HIGH);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, HIGH);
  analogWrite(MotorRightPWM, 100);
  analogWrite(MotorLeftPWM, iSpeed);
  delay(d * 10);
}
void left(int d) { //turn right (single wheel)
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, HIGH);
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, LOW);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);
  delay(d * 10);
}
void right(int d) {//turn left(single wheel)
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, HIGH);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, LOW);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);
  delay(d * 10);
}
void turnR(int d) {//turn right (two wheels)
  digitalWrite(MotorRight1, HIGH);
  digitalWrite(MotorRight2, LOW);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, HIGH);
  analogWrite(MotorRightPWM, 255);
  analogWrite(MotorLeftPWM, 255);  
  delay(d * 10);
}
void turnL(int d) {//turn left (two wheels)
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, HIGH);
  digitalWrite(MotorLeft1, HIGH);
  digitalWrite(MotorLeft2, LOW);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);
  delay(d * 10);
}
void stopp(int d) { //stop
  digitalWrite(MotorRight1, LOW);
  digitalWrite(MotorRight2, LOW);
  digitalWrite(MotorLeft1, LOW);
  digitalWrite(MotorLeft2, LOW);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);  
  delay(d * 10);
}
void back(int d) { //go back
  digitalWrite(MotorRight1, HIGH);
  digitalWrite(MotorRight2, LOW);
  digitalWrite(MotorLeft1, HIGH);
  digitalWrite(MotorLeft2, LOW);
  analogWrite(MotorRightPWM, iSpeed);
  analogWrite(MotorLeftPWM, iSpeed);  
  delay(d * 10);
}

