
#include<Servo.h>

Servo motor;


const int Audiosensor = A0; 
const int led = 13;
 

const int threshold = 500;
const int rejector = 25;            //individueller knock offset -prozent
const int averagerejector = 15;     //durchschnitt timing offset- prozent
const int knockfade = 100;          //fading zwischen knocks
const int knocktimer = 1200;        //kommen noch knocks?
const int maxknock = 20;

int codearray[maxknock] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //shave and a haircut-default
//int codearray[maxknock] = {50, 50, 50, 25, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  //funky cold medina
//int codearray[maxknock] = {57, 61, 54, 35, 56, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
int codearraytemp[maxknock];
int AudiosensorValue = 0;
boolean highlow =false;
boolean lichtan = true;
int posan = 1;
int posaus = 0;



void setup() {

  Serial.begin(9600);      //DEBUG             
  Serial.println("Start"); //DEBUG
  
  pinMode(led, OUTPUT);
  motor.attach(8);

  motor.write(90);

}

void loop() {

  AudiosensorValue = analogRead(Audiosensor);

  if(AudiosensorValue <=threshold)
 {
  highlow = true;
  }
  else { highlow =false;}
  
  if (highlow ==true){
    listenToKnock();
  }
/*if(lichtan == true){
  motor.write(posan);
  }
else{motor.write(posaus);}*/
  
} 




void listenToKnock(){
  Serial.println("knock start");

  int i = 0;
  for (i=0;i<maxknock;i++){
    codearraytemp[i]=0;
  }
  
  int knockNumbertemp=0;
  int starttime=millis();
  int now=millis();

  delay(knockfade);

  do {
    //nächter knock?
    AudiosensorValue = analogRead(Audiosensor);
   if(AudiosensorValue <=threshold){
    highlow = true;}
   else { highlow =false;}
  
  if (highlow ==true){
      //Serial.println(AudiosensorValue); //DEBUG
     
      now=millis();
      codearraytemp[knockNumbertemp] = now-starttime;
      int debug =(now-starttime)/6; //DEBUG
      Serial.println(debug); //DEBUG
      knockNumbertemp ++;
      starttime=now;          
      delay(knockfade);
    }

    now=millis();
  } while ((now-starttime < knocktimer) && (knockNumbertemp < maxknock));//Timeout check - max knock check
  

  if (validate() == true){
      trigger(); 

      Serial.println("knock accepted");
    }
}

void trigger(){
  if(lichtan == true){
    
     motor.write(180);
      delay(100);
      motor.write(90);
     
    digitalWrite(led, HIGH);
    lichtan = false;}
    else{
     motor.write(30);
    delay(100);
     motor.write(90);
      
      lichtan = true;
      digitalWrite(led, LOW);}
  //int i=0;

  //motor.write(10);
  //delay
  //motor.write(50);
  
  // blinken
  //for (i=0; i < 5; i++){   
      //digitalWrite(led, LOW);
     // delay(100);
     
     // delay(100);
 // }
   
}

// knockcheck - true/false
boolean validate(){
  int i=0;
 
  //anzahl
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;
  
  for (i=0;i<maxknock;i++){
    if (codearraytemp[i] > 0){
      currentKnockCount++;
    }
    if (codearray[i] > 0){ 
      secretKnockCount++;
    }
    
    if (codearraytemp[i] > maxKnockInterval){
      maxKnockInterval = codearraytemp[i];
    }
  }
  if (currentKnockCount != secretKnockCount){
    return false; 
  }
  
  //knock tempo relativ 
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (i=0;i<maxknock;i++){
    codearraytemp[i]= map(codearraytemp[i],0, maxKnockInterval, 0, 100);      
    timeDiff = abs(codearraytemp[i]-codearray[i]);
    if (timeDiff > rejector){
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  //nächster check
  if (totaltimeDifferences/secretKnockCount>averagerejector){
    return false; 
  }
  
  return true;
  
}

