//Imported Libraries
#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>

//Wheel Constants
#define SPEEDSIDE 240   //The speed with which the rover takes the turn
#define SPEEDFRONT 165  //The speed with which the rover moves
#define WAIT 300        //Delay for the rover to turn 
#define SERVOWAIT 1000  //Time period for which the Servo stays at a particular position
#define WAITLEFT 490
#define WAITRIGHT 600

//Servo Constants
#define LEFT 180    //At this angle, the ultrasonic sensor looks left
#define RIGHT 0    //At this angle, the ultrasonic sensor looks right
#define AHEAD 90    //Default angle of the Servo motor. At this angle, the ultrasonic sensor looks ahead

//UltraSonic sensor constants
#define TRIGGER A0    //Analog pin 5 of arduino is connected to the "Trigger" of the Ultrasonic sensor
#define ECHO A1       //Analog pin 0 of arduino is connected to the "Echo" of the Ultrasonic sensor
#define THRESHOLD 20  //The distance of 20cm is taken as the threshold distance

//Pump variables
#define PUMPDELAY 800
#define PUMPSPEED 190

//Motor Variables
AF_DCMotor right(4, MOTOR34_64KHZ);
AF_DCMotor left(3, MOTOR34_64KHZ);
AF_DCMotor pump(1, MOTOR12_1KHZ);

//Servo
Servo angle; 

//UltraSonic Sensor
NewPing detect(TRIGGER, ECHO, 400); //default value -> 500cm
#define TIMEOUT 200

/*Direction Control 
(Left -> -1 || Right -> 1)*/
int controlVal = -1;
int controlNum = 0;

void setup() 
{
    //Speed Configuration
    right.setSpeed(255);
    left.setSpeed(255);

    delay(500);

    right.setSpeed(SPEEDFRONT);
    left.setSpeed(SPEEDFRONT);
      
    //Setting Servo
    angle.attach(10);
    angle.write(AHEAD);

    //First Pump Push
    pump.run(FORWARD);
    pump.setSpeed(PUMPSPEED);
    delay(3*PUMPDELAY);
    pump.setSpeed(0);

    //Debugging
    Serial.begin(1200);
}

void loop() 
{
    if(obstacleAhead())
    {
        moveStop();
        
        if(isControlPathBlocked())
        {
            Serial.println("Control Path is blocked");
            controlNum += 1;
            controlVal *= -1;
            turnRover();
        }

        else
        {
            Serial.println("Way to go!!");
            turnRover();
            delay(300);
            
            moveForward();
            delay(1000);
            
            moveStop();
            turnRover();
            
            controlVal *= -1;
        }

        if(controlNum >= 2)
        {
          left.setSpeed(0);
          right.setSpeed(0);
          exit(0);
        }

        pourWater();

        moveForward();
    }

    else
      moveForward();
}

//Moves Right
void moveRight()
{
    Serial.println("Moving Right!");
    moveStop();
    delay(WAIT);
   
    left.run(FORWARD);
    right.run(BACKWARD);
    speedUp(SPEEDSIDE);
    delay(WAITRIGHT);
    
    moveStop();
}

//Moves Left
void moveLeft()
{
    Serial.println("Moving left!");
    moveStop();
    delay(WAIT);
    
    left.run(BACKWARD);
    right.run(FORWARD);
    speedUp(SPEEDSIDE);
    delay(WAITLEFT);
    
    moveStop();
}

//Moves Front
void moveForward()
{
    Serial.println("Going forward");
    
    left.run(FORWARD);
    right.run(FORWARD);
    
    speedUp(SPEEDFRONT);
}

//Moves Back
void moveBackward()
{
    Serial.println("Going Back");
      
    left.run(BACKWARD);
    right.run(BACKWARD);
    
    speedUp(SPEEDFRONT);
}

void speedUp(int speed)
{
    left.setSpeed(speed);
    right.setSpeed(speed);
}

//Slows Down + Stops Moving
void moveStop()
{
    Serial.println("...Stopped");
   
    left.setSpeed(0);
    right.setSpeed(0);
    
    delay(WAIT);
}

//Obstacle detection
//Returns 1 if there is an obstacle is detected by the Ultrasonic sensor
int obstacleAhead()
{
  int obstacleDistance = detect.ping_cm();
  Serial.println(obstacleDistance);//Debugging
  while(obstacleDistance == 0)
  {
    pinMode(ECHO,OUTPUT);
    digitalWrite(ECHO,LOW);
    delay(TIMEOUT);
    pinMode(ECHO,INPUT);
    obstacleDistance = detect.ping_cm();
    Serial.println(obstacleDistance);//Debugging
  }

  if(obstacleDistance <= THRESHOLD )
  {
    Serial.println("Obstacle Ahead!!!");
    return 1;
  }

  return 0;
}

int obstacleOnLeft()  
{
  angle.write(LEFT);
  delay(SERVOWAIT);

  int obstacleDistance = detect.ping_cm();
  Serial.println(obstacleDistance);//Debugging
  while(obstacleDistance == 0)
  {
    pinMode(ECHO,OUTPUT);
    digitalWrite(ECHO,LOW);
    delay(TIMEOUT);
    pinMode(ECHO,INPUT);
    obstacleDistance = detect.ping_cm();
    Serial.println(obstacleDistance);//Debugging
  }
  
  angle.write(AHEAD);

  if(obstacleDistance <= THRESHOLD)
    return 1;

  return 0;  
}

int obstacleOnRight()  
{
  angle.write(RIGHT);
  delay(SERVOWAIT);

  int obstacleDistance = detect.ping_cm();
  Serial.println(obstacleDistance);//Debugging
  while(obstacleDistance == 0)
  {
    pinMode(ECHO,OUTPUT);
    digitalWrite(ECHO,LOW);
    delay(TIMEOUT);
    pinMode(ECHO,INPUT);
    obstacleDistance = detect.ping_cm();
    Serial.println(obstacleDistance);//Debugging
  }
  
  angle.write(AHEAD);

  if(obstacleDistance <= THRESHOLD)
    return 1;

  return 0;  
}

//Checks if the Control Path is Blocked
int isControlPathBlocked()
{
    if(controlVal == -1)
        return obstacleOnLeft();
    
    return obstacleOnRight();
}

//Turns the Rover in Control Path
void turnRover()
{
    if(controlVal == -1)
      moveLeft();
    else
       moveRight();
}

//Pumps Water
void pourWater()
{
  //Starting Pump
  pump.setSpeed(PUMPSPEED);
  delay(PUMPDELAY);

  //Stopping it
  pump.setSpeed(0);
}
