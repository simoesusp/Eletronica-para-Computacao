#include <SoftwareSerial.h>
#include <PWM.h>

#include <NewPing.h>

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PINA     3  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define ECHO_PINB     4  // Arduino pin tied to echo pin on the ultrasonic sensor.

#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonarA(TRIGGER_PIN, ECHO_PINA, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarB(TRIGGER_PIN, ECHO_PINB, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

long durationA, distanceA;
long durationB, distanceB;

int Sensor1, Sensor2, M1, M2;

int targetVelocidade = 0;
int L=0;
int R=0;
int count = 0;

int LED = 13;

int M1PWM = 9;
int M1DirA = 8;
int M1DirB = 6;


int M2PWM = 10;
int M2DirA = 7;
int M2DirB = 5;

int32_t frequency = 50;
bool successM1 = false;
bool successM2 = false;

SoftwareSerial mySerial(11, 12); // RX, TX

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(M1DirA, OUTPUT);
  pinMode(M1DirB, OUTPUT);
  pinMode(M2DirA, OUTPUT);
  pinMode(M2DirB, OUTPUT);
   
  digitalWrite(LED, LOW);    
  digitalWrite(M1DirA, HIGH); 
  digitalWrite(M1DirB, LOW);
  digitalWrite(M2DirA, HIGH); 
  digitalWrite(M2DirB, LOW);
  
  InitTimersSafe();
  successM1 = SetPinFrequencySafe(M1PWM, frequency);
  successM2 = SetPinFrequencySafe(M2PWM, frequency);

  if(successM1 && successM2) 
    digitalWrite(LED, HIGH);    
  
  pwmWrite(M1PWM, 0);
  pwmWrite(M2PWM, 0);
  
  Serial.begin(9600);
  mySerial.begin(9600);
} 
 
void loop() {
Loop:;  
  count++;
  if (count > 1000) // Se contador chegar a 1 seg stop robot
        {
         pwmWrite(M1PWM, 0);
         pwmWrite(M2PWM, 0); 
         count = 0;
        }
        
  delay(1);
  
  if (mySerial.available()) {
          //delay(10);
          targetVelocidade = (mySerial.read());
          L = targetVelocidade/16;
          R = targetVelocidade - L*16;

          if((L==5) && (R==5))  goto Automato;

          if(L>5 || R>5)
              {
               if(L>5)  L=L-5;               
               if(R>5)  R=R-5; 
               
               digitalWrite(M1DirA, LOW); 
               digitalWrite(M1DirB, HIGH);
               digitalWrite(M2DirA, LOW); 
               digitalWrite(M2DirB, HIGH);
               
               digitalWrite(LED, LOW);    
              }
          else
              {
               digitalWrite(M1DirA, HIGH);  
               digitalWrite(M1DirB, LOW);
               digitalWrite(M2DirA, HIGH); 
               digitalWrite(M2DirB, LOW);
                
               digitalWrite(LED, LOW);                    
              }
        
          if(((L == R) || (L==R+1) || (L==R+2) || (L==R+3) || (L==R+4) || (L==R+5)) || ((R==L+1) || (R==L+2) || (R==L+3) || (R==L+4) || (R==L+5))) 
              {
//             if(distance>100){
                  pwmWrite(M1PWM, (51*R));
                  pwmWrite(M2PWM, (51*L));
/*                  }
               else {    
                  pwmWrite(M1PWM, (0));
                  pwmWrite(M2PWM, (0));
                  }
  */                
               count = 0;  // Se recebeu comando valido, Zera contador
                  
              }

      } 
      
  if (Serial.available() > 0) {
          delay(10);
          targetVelocidade = (Serial.read());
          Serial.write(targetVelocidade);
          
          L = targetVelocidade/16;
          R = targetVelocidade - L*16;
          
          pwmWrite(M1PWM, (51*L));
          pwmWrite(M2PWM, (51*R));
          
          count = 0;
      }       

goto Loop;

Automato:;
   digitalWrite(LED, HIGH);                    

// Le Sensores
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  unsigned int uS = sonarA.ping(); // Send ping, get ping time in microseconds (uS).
  uS = uS / US_ROUNDTRIP_CM; // Convert ping time to distance in cm and print result (0 = outside set distance range)
  if(uS<50 && uS>0) Sensor1 = 1;
    else    Sensor1 = 0;
  
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  uS = sonarB.ping(); // Send ping, get ping time in microseconds (uS).
  uS = uS / US_ROUNDTRIP_CM; // Convert ping time to distance in cm and print result (0 = outside set distance range)
  if(uS<50 && uS>0) Sensor2 = 1;
    else    Sensor2 = 0;
    
// IA   
  if(Sensor1==0 && Sensor2==0)  {M1=1;  M2=1;} 
  else if(Sensor1==1 && Sensor2==0)  {M1=1;  M2=0;}
  else if(Sensor1==0 && Sensor2==1)  {M1=0;  M2=1;}
  else if(Sensor1==1 && Sensor2==1)  {M1=0;  M2=1;}

// Drive Motores
  digitalWrite(M1DirA, M1); 
  digitalWrite(M1DirB, !M1);
  digitalWrite(M2DirA, M2); 
  digitalWrite(M2DirB, !M2);

  if (mySerial.available()) {
          targetVelocidade = (mySerial.read());
          L = targetVelocidade/16;
          R = targetVelocidade - L*16;

          if((L==0) && (R==0))  goto Loop;
          }
  goto Automato;



} 

