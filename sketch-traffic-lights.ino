/*
  Sketch - Arduino Traffic Lights
  https://github.com/zubial
*/

// Hardware settings
const int redPin = 2;         // Red Light DigitalPin
const int greenPin = 3;       // Green Light DigitalPin
const int yellowPin = 4;      // Yellow Light DigitalPin

const int triggerPin = 10;    // Trigger DigitalPin
const int echoPin = 9;        // Echo DigitalPin

const int distancePin = A0;      // Distance setting AnalogPin

// Measurments
long detectDistance = 0;
long presenceDuration = 0;

// Configuration
const long changeStateDelay = 3;    // Delay before change state (seconds)
const long longPresenceDelay = 120;  // Delay after is long presence (seconds)
const long shortOutDuration = 20;   // Out short duration (seconds)
const long longOutDuration = 180;    // Out long duration (seconds)
long presenceDistance = 0;

void setup() {
  Serial.begin (9600);
  
  // TrafficLights
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
    
  // Config HC-SR04
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  Serial.println("System starting ... ");
  delay(3000);
  
  // Settings
  presenceDistance = getVariable(distancePin, 0, 400);
 
  turnOnLight(greenPin);
  Serial.println("System started ... ");
  
  Serial.print("Distance cm : ");
  Serial.println(presenceDistance);
}

void loop() {

  if (wait4Present()) {
    // Debug print
    Serial.println("Somebody is coming ... ");
    Serial.print(detectDistance);
    Serial.print(" / ");
    Serial.print(presenceDistance);
    Serial.println(" cm");
  
    turnOnLight(redPin);
    delay(10000);
    
    if (wait4Leave() < (longPresenceDelay*1000)) {
      Serial.println("Somebody is leaving ... ");
      outProcess(shortOutDuration * 1000);
      
    } else {
      Serial.println("Somebody left a gift ... ");
      outProcess(longOutDuration * 1000);
      
    }
  }
  
  delay(500);  
}

bool wait4Present() {
  int count = 0;
  
  while(count < changeStateDelay) {
    detectDistance = getDistance();
    
    while(detectDistance > presenceDistance && detectDistance > 0) {
      detectDistance = getDistance();
      count = 0;
      delay(1000); 
    }
    count++;
    delay(1000);
  }
  return true;
}

long wait4Leave() {
  int count = 0;
  
  while(count < changeStateDelay) {
    detectDistance = getDistance();

    while(detectDistance < presenceDistance && detectDistance > 0) {
      detectDistance = getDistance();
      count = 0;
      presenceDuration++;
      delay(1000); 
    }
    count++;
    delay(1000);
  }
  
  return (presenceDuration*1000);
}

long getDistance() {
  // Send pulse
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(5);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  
  // Receive echo
  long duration = pulseIn(echoPin, HIGH);
  
  // Convert the time into a distance
  return (duration/2) / 29.1; 
}

void outProcess(long outDuration) {
  turnOnLight(yellowPin);
  delay(outDuration);
  turnOnLight(greenPin);
}

void turnOffLight() {
  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(yellowPin, LOW);
}

void turnOnLight(int light) {
  turnOffLight();
  
  digitalWrite(light, HIGH);
}

int getVariable(int valuePin, int min, int max) {  
  float value = analogRead(valuePin);
  
  return map(value, 0, 1023, min, max);
}

