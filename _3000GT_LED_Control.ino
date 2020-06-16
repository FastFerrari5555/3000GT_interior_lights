//WHITE TIMEOUT SETTINGS
long startmillis;
bool killWhite = false;
int previousWhite;
int killTime = 10000; //ten second limit to white in off mode

//OUTPUTS
int ledRed = 3;
int ledWhite = 5;

//INPUTS
int doorSwitch = 6;
int switchManual = 7;
int switchAuto = 8;

//RED AVERAGE VARIABLES
const int numRedReadings = 400;       //Number of data points for averagers
int redReadings[numRedReadings];      // the redReadings from the analog input
int redReadIndex = 0;              // the index of the current reading
float redTotal = 0;                  // the running redTotal
float redAverage = 0;                // the redAverage

//WHITE AVERAGE VARIABLES
const int numWhiteReadings = 400;       //Number of data points for averagers
int whiteReadings[numWhiteReadings];      // the whiteReadings from the analog input
int whiteReadIndex = 0;              // the index of the current reading
float whiteTotal = 0;                  // the running whiteTotal
float whiteAverage = 0;                // the whiteAverage

void setup() 
{ 
 Serial.begin(115200);
 pinMode(ledRed, OUTPUT);
 pinMode(ledWhite, OUTPUT);
 
 pinMode(doorSwitch, INPUT_PULLUP);
 pinMode(switchManual, INPUT);
 pinMode(switchAuto, INPUT);

 previousWhite = digitalRead(doorSwitch)*255;
 
 for (int thisReading = 0; thisReading < numRedReadings; thisReading++) { //switch all of the array vars to 0 by default
    redReadings[thisReading] = 0;
  }
 for (int thisReading = 0; thisReading < numWhiteReadings; thisReading++) { //switch all of the array vars to 0 by default
    whiteReadings[thisReading] = 0;
  }
} 

void loop() {
 int doorSwitchPWM = digitalRead(doorSwitch)*255;
 //Serial.println(doorSwitchPWM);

 if (digitalRead(switchAuto) == HIGH){ //auto, red if door is closed
    killWhite = false;
    
    writeRedSmooth(doorSwitchPWM);
    writeWhiteSmooth(255-doorSwitchPWM); //enables white when door is open
 }
 else if (digitalRead(switchManual) == HIGH) { //manual, full red
    killWhite = false;
    writeRedSmooth(255);
    writeWhiteSmooth(255); //enables white manually while key is in
 }
 else { //off mode
  Serial.println(255-doorSwitchPWM);
  if(255-doorSwitchPWM == 255) { //RESET YOUR SHIT WHEN YOU CLOSE THE DOOR
    if (255-previousWhite == 0){
      Serial.println("Close Door");
      killWhite = false;
    }
  }
  else if (255-previousWhite == 0){ //WHEN DOOR OPENS, DO SHIT
    if(255-doorSwitchPWM == 255) {
      Serial.println("Open Door");
      startmillis = millis();
    }
    startmillis = millis();
  }
  if((millis() - startmillis) > killTime) {
    killWhite = true;
    Serial.println("KILL");
  }
  
  previousWhite = doorSwitchPWM;
  writeRedSmooth(0);
  writeWhiteSmooth(255-doorSwitchPWM); //enables white when door is open
 }

 delay(2);        // delay in between reads for stability
}

void writeRedSmooth(int inputPWM) {
 redTotal = redTotal - redReadings[redReadIndex]; // subtract the last reading
 redReadings[redReadIndex] =  inputPWM;// read from the sensor
 redTotal = redTotal + redReadings[redReadIndex]; // add the reading to the redTotal
 redReadIndex = redReadIndex + 1; // advance to the next position in the array
 if (redReadIndex >= numRedReadings) { // if we're at the end of the array
   // ...wrap around to the beginning:
   redReadIndex = 0;
 }
 redAverage = redTotal / numRedReadings; // calculate the redAverage
 //analogWrite(ledRed, redAverage);
 expAnalogWrite(ledRed, redAverage);
}

void writeWhiteSmooth(int inputPWM) {
 whiteTotal = whiteTotal - whiteReadings[whiteReadIndex]; // subtract the last reading
 whiteReadings[whiteReadIndex] =  inputPWM;// read from the sensor
 whiteTotal = whiteTotal + whiteReadings[whiteReadIndex]; // add the reading to the whiteTotal
 whiteReadIndex = whiteReadIndex + 1; // advance to the next position in the array
 if (whiteReadIndex >= numWhiteReadings) { // if we're at the end of the array
   // ...wrap around to the beginning:
   whiteReadIndex = 0;
 }
 whiteAverage = whiteTotal / numWhiteReadings; // calculate the redAverage
 //analogWrite(ledWhite, whiteAverage);
 
 if (killWhite == true) {
  expAnalogWrite(ledWhite, 0); //if killed write 0
 }
 else {
  expAnalogWrite(ledWhite, whiteAverage); //if killed, write what it should be, like normal
 }
}

void expAnalogWrite(int led, int input) {
  float output = pow(input,2)/255;
  //Serial.println(String(input) + " " + String(output));
  analogWrite(led, round(output));
}
