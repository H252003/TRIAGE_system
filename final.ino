/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
 
#define REPORTING_PERIOD_MS 1000


int lvl = 0;
int spoVal ;
PulseOximeter pox;
uint32_t tsLastReport = 0;
 
void onBeatDetected()
{
    Serial.println("Beat!");
}

// TCS230 or TCS3200 pins wiring to Arduino
//#define LED 3
// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
int id ;

// Calibration Values
// *Get these from Calibration Sketch
int redMin = 26 ; // Red minimum value
int redMax = 164; // Red maximum value
int greenMin = 23; // Green minimum value
int greenMax = 177; // Green maximum value
int blueMin = 21; // Blue minimum value
int blueMax = 144
; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for final Color values
int redValue;
int greenValue;
int blueValue;

boolean normal = false;

void setup()
{
    Serial.begin(115200);
    Serial.print("Initializing pulse oximeter..");
 
    // Initialize the PulseOximeter instance
    // Failures are generally due to an improper I2C wiring, missing power supply
    // or wrong target chip
    
    if (!pox.begin()) {
        Serial.println("FAILED");
        for(;;);
    } else {
        Serial.println("SUCCESS");
    }    
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
 
    // Register a callback for the beat detection
    // pox.setOnBeatDetectedCallback(onBeatDetected);

     // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  //pinMode(LED, OUTPUT);

  // Set Sensor output as input
  pinMode(sensorOut, INPUT);

  // Set Frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);
  //digitalWrite(LED,HIGH);

  // Setup Serial Monitor
//  Serial.begin(115200);
}

void loop()
{

    // Make sure to call update as fast as possible
    pox.update();
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        Serial.print("Heart rate:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        spoVal = pox.getSpO2();
        Serial.println("%");
        colorSens();
 
        tsLastReport = millis();
    }
}

void colorSens() {
  
  
  // Read Red value
  redPW = getRedPW();
  // Map to value from 0-255
  redValue = map(redPW, redMin,redMax,255,0);
  // Delay to stabilize sensor
  //delay(200);

  // Read Green value
  greenPW = getGreenPW();
  // Map to value from 0-255
  greenValue = map(greenPW, greenMin,greenMax,255,0);
  // Delay to stabilize sensor
  //delay(200);

  // Read Blue value
  bluePW = getBluePW();
  // Map to value from 0-255
  blueValue = map(bluePW, blueMin,blueMax,255,0);
  // Delay to stabilize sensor
  //delay(200);

  check();
  
  getLevel(id,pox.getHeartRate());
  addSpo2(spoVal,lvl);
  Serial.print("level is: ");
  Serial.println(lvl);
 
  Serial.print("Red = ");
  Serial.print(redValue);
  Serial.print(" - Green = ");
  Serial.print(greenValue);
  Serial.print(" - Blue = ");
  Serial.println(blueValue);

  id=0;
  
}



// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2,LOW);
  digitalWrite(S3,LOW);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2,HIGH);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2,LOW);
  digitalWrite(S3,HIGH);
  // Define integer to represent Pulse Width
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

void check(){
  if (redValue>=175 && redValue<=240 && greenValue>=140 && greenValue<=215 && blueValue>=105 && blueValue<=155 && redValue > (greenValue +20 ) && greenValue > (blueValue + 20) ){
    Serial.println("Jaundes");
    id = 1 ;
    }
  else if (redValue>=110&&redValue<=202&& greenValue>=90&&greenValue<=155 && blueValue>=80 && blueValue<=164 && blueValue< (redValue -10) && blueValue<=(greenValue + 10) && blueValue>=(greenValue - 10)&& redValue<(greenValue + 40)){
    Serial.println("normal");
    id = 2 ;
    }
  else if ((redValue>=115 && redValue<=193 && greenValue>=100 && greenValue<=147 && blueValue>=135 && blueValue<=220 && blueValue>(greenValue + 17) )){
    Serial.println("Scyanosis");
    id = 3 ;
  }
  else if ((redValue>=115 && redValue<=180 && greenValue>=70 && greenValue<=105 && blueValue>=70 && blueValue<=105 && blueValue<=(greenValue + 10) && blueValue>=(greenValue - 10) && redValue>(greenValue + 40) )){
    Serial.println("Redness");
    id = 4;
  }
  else if ((redValue>=175 && redValue<=242 && greenValue>=156 && greenValue<=200 && blueValue>=140 && blueValue<=215  )){
    Serial.println("pale");
    id = 5;
  }
  
}
 
void getLevel(int id, int pulse){
  int pulselevel = (pulse < 70)?-1:(pulse < 100)?0:1; 
  if(id == 1){
    if(pulselevel == 1)
    {
      lvl = 2;
        
    }
    if(pulselevel == 0)
    {
        lvl = 2;    
    }
    if(pulselevel == -1)
    {
      Serial.println("level 2");
    }
  }
   if(id == 2){
    if(pulselevel == 1)
    {
        lvl = 4;
    }
    if(pulselevel == 0)
    {
      lvl = 1;
    }
    if(pulselevel == -1)
    {
      lvl = 3;
    }
  }
   if(id == 3){
    if(pulselevel == 1)
    {
        lvl = 4;
    }
    if(pulselevel == 0)
    {
      lvl = 2;
    }
    if(pulselevel == -1)
    {
      lvl = 4;
    }
  }
   if(id == 4){
    if(pulselevel == 1)
    {
        lvl = 4;
    }
    if(pulselevel == 0)
    {
      lvl = 2;
    }
    if(pulselevel == -1)
    {
      lvl = 3;
    }
  }
   if(id == 5){
    if(pulselevel == 1)
    {
        lvl = 4;
    }
    if(pulselevel == 0)
    {
      lvl = 1;
    }
    if(pulselevel == -1)
    {
      lvl = 3;
    }
  }

}

void addSpo2(int spoVal,int lvl)
{
  if(spoVal < 70)
  lvl += 2;
  else if(spoVal < 85)
  lvl += 1;
  
}


 
