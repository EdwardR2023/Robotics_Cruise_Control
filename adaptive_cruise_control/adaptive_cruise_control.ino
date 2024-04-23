#include <Servo.h>
Servo myservo;
#define TRIG_PIN A1  // ultrasonic module  TRIG to A1
#define ECHO_PIN A0  // ultrasonic module   ECHO to A0
#define Lpwm_pin  5     //pin of controlling speed---- ENA of motor driver board
#define Rpwm_pin  6    //pin of controlling speed---- ENB of motor driver board
int pinLB=2;             //pin of controlling turning---- IN1 of motor driver board
int pinLF=4;             //pin of controlling turning---- IN2 of motor driver board
int pinRB=7;            //pin of controlling turning---- IN3 of motor driver board
int pinRF=8;            //pin of controlling turning---- IN4 of motor driver board
int ult_distance=0; 
int pos=0;

/* 
1. Scan 180 degrees in front of bot
2. If (object_dist < 50cm && object_dist > 0cm): 
    myservo.write(90); 
    turn bot toward object
3. If (object_dist > 20cm)
    move bot forward
   Else If (object_dist < 20 cm)
    move bot backwards
*/

// This is the set up code, shouldn't have to worry about this.
void setup() 
{
  myservo.attach(A2);
  Serial.begin(9600);
  myservo.write(90);
  pinMode(TRIG_PIN, OUTPUT);    
  pinMode(ECHO_PIN, INPUT);      
  pinMode(pinLB,OUTPUT); // /pin 2
  pinMode(pinLF,OUTPUT); // pin 4
  pinMode(pinRB,OUTPUT); // pin 7
  pinMode(pinRF,OUTPUT);  // pin 8
  pinMode(Lpwm_pin,OUTPUT);  // pin 5 (PWM) 
  pinMode(Rpwm_pin,OUTPUT);  // pin 6(PWM)   
}

// This calls the main function repeatedly 
void loop() 
{
  ultrasonic_servo();
}

// This determies the distance an object is from the bot
float check_distance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  float distance = pulseIn(ECHO_PIN, HIGH) / 58.00;
  delay(10);
  Serial.print("Distance:");
  Serial.print(distance);
  Serial.println("CM");
  delay(100);
  return distance;
}

// In this function the robot tries to keep whatever object it is following
// exactly 20cm away from it. 
void cruise_control()
{
  int cruise_distance = check_distance(); 
  if (cruise_distance > 20 && cruise_distance < 100)
  {
    go_forward(100); 
  }
  else if (cruise_distance < 20 && cruise_distance > 0)
  {
    go_backward(100);
  }
  else if (cruise_distance == 20)
  {
    stop();
  }
  else 
  {
    loop();
  }

  cruise_control();
}

// determines direction the bot must turn to face object
// and then turns till it faces the object. 
void turn_bot(int distance, int degree)
{
  int curr_distance = 0; 
  int u_distance = distance + 15; 
  int l_distance = distance - 15; 

  if (degree >= 90)
  {
    while (curr_distance < l_distance || curr_distance > u_distance)
    {
      rotate_right(125);
      delay(100);
      curr_distance = check_distance();
    }
    stop();
    delay(100);

    cruise_control();
  }
  else {
    while (curr_distance < l_distance || curr_distance > u_distance)
    {
      rotate_left(75);
      delay(100);
      curr_distance = check_distance();
    }
    stop(); 
    delay(100); 

    cruise_control(); 
  }
}

// Determines whether or not you follow the object in front of the bot
void determine_follow(int distance, int degree) 
{
  // determines if object is close enough to follow
  if ((distance <= 30) && (distance >= 0))
  {
    myservo.write(90);
    delay(20); 
    turn_bot(distance, degree);
  } 
  else 
  {
    return;
  }
}

// This makes the ultrasonic sensor turn from 0-180 degrees, scanning as it goes
void ultrasonic_servo() 
{
  for (pos = 0; pos <= 180; pos++) 
  { // goes from 0 degrees to 180 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //delay(10);                       // waits 15ms for the servo to reach the position

    ult_distance = check_distance();
    

    determine_follow(ult_distance, pos);
  }

  for (pos = 180; pos >= 0; pos--) 
  { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //delay(10);                       // waits 15ms for the servo to reach the position

    ult_distance = check_distance();

    determine_follow(ult_distance, pos);
  }
}

void go_forward(unsigned char speed_val)    // speed_val：0~255
{
  digitalWrite(pinRB,HIGH); 
  digitalWrite(pinRF,LOW);
  digitalWrite(pinLB,HIGH);
  digitalWrite(pinLF,LOW);
  analogWrite(Lpwm_pin,speed_val);
  analogWrite(Rpwm_pin,speed_val); 
}

void go_backward(unsigned char speed_val)    // speed_val：0~255
{
  digitalWrite(pinRB,LOW);  
  digitalWrite(pinRF,HIGH);
  digitalWrite(pinLB,LOW);  
  digitalWrite(pinLF,HIGH);
  analogWrite(Lpwm_pin,speed_val);
  analogWrite(Rpwm_pin,speed_val);
}
    
void rotate_left(unsigned char speed_val)        // speed_val：0~255
{
  digitalWrite(pinRB,HIGH);
  digitalWrite(pinRF,LOW );  
  digitalWrite(pinLB,LOW); 
  digitalWrite(pinLF,HIGH);
  analogWrite(Lpwm_pin,speed_val);
  analogWrite(Rpwm_pin,speed_val);
}

void rotate_right(unsigned char speed_val)    // speed_val：0~255
{
  digitalWrite(pinRB,LOW);  
  digitalWrite(pinRF,HIGH);
  digitalWrite(pinLB,HIGH);
  digitalWrite(pinLF,LOW);  
  analogWrite(Lpwm_pin,speed_val);
  analogWrite(Rpwm_pin,speed_val);
}  

void stop()        //stop
{
  digitalWrite(pinRB,HIGH);
  digitalWrite(pinRF,HIGH);
  digitalWrite(pinLB,HIGH);
  digitalWrite(pinLF,HIGH);
}
