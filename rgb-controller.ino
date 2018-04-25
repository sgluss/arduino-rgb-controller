// timers used for non-blocking scheduling
unsigned long colorSetTime = 0;
unsigned long encoderMoveTime = 0;
unsigned long currentTime = 0;

// Set LED pins
const int RED_LED_PIN = 3;
const int GREEN_LED_PIN = 10;
const int BLUE_LED_PIN = 11;

// Set Rotary Encoder pins
const int ROTARY_SW_PIN = 8;
const int ROTARY_DT_PIN = 6;
const int ROTARY_CLK_PIN = 5;

// Set pot pins
const int SATURATION_POT_PIN = 17;
const int BRIGHTNESS_POT_PIN = 14;

// radial coordinates for color wheel
// 0 - 360
double hue = 0;
// 0 - 1 (higher value is less saturation, lower value raises the minimum value for all colors)
double saturation = 0.99;
// 0 - 1 (brightness control, 0 is min, 1 is max
double brightness = 1.0;

// 0 - 255
int redVal = 255;
int greenVal = 255;
int blueVal = 255;

int rotaryStateSW = HIGH;
int rotaryStateDT = HIGH;
int rotaryStateCLK = HIGH;

int rotaryStateSWNew = HIGH;
int rotaryStateDTNew = HIGH;
int rotaryStateCLKNew = HIGH;

// stores whether the encoder could be between clicks
int encoderPossiblyBetween = 0;
// if the encoder has gone high, but DT or CLK are still high, leave this set
int encoderIsTurning = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  pinMode(ROTARY_SW_PIN, INPUT);
  pinMode(ROTARY_DT_PIN, INPUT);
  pinMode(ROTARY_CLK_PIN, INPUT);
  
  pinMode(BRIGHTNESS_POT_PIN, INPUT);
  pinMode(SATURATION_POT_PIN, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();
  
  checkButtonStates();

  checkPotStates();

  // set new color every 50 millis
  if (currentTime - colorSetTime > 50) {
    colorSetTime = currentTime;
    setNewColor(1);
  }
}

void checkButtonStates() {
  rotaryStateSWNew = digitalRead(ROTARY_SW_PIN);
  rotaryStateDTNew = digitalRead(ROTARY_DT_PIN);
  rotaryStateCLKNew = digitalRead(ROTARY_CLK_PIN);

  encoderPossiblyBetween = rotaryStateDTNew == LOW || rotaryStateCLKNew == LOW;

  // pushbutton state machine
  if (rotaryStateSWNew == LOW && rotaryStateSW == HIGH) {
    rotaryStateSW = LOW;
    encoderPress();
  } else if (rotaryStateSWNew == HIGH && rotaryStateSW == LOW) {
    // reset state
    rotaryStateSW = HIGH;
  } 

  // turning encoder to the left state machine
  if (rotaryStateDTNew == LOW && encoderIsTurning == 0 && rotaryStateDT == HIGH) {
    rotaryStateDT = LOW;
    encoderIsTurning = 1;
    encoderLeft();
  } else if (rotaryStateDT == LOW && encoderPossiblyBetween == false){
    rotaryStateDT = HIGH;
    encoderIsTurning = 0;
  }

  // turning encoder to the right state machine
  if (rotaryStateCLKNew == LOW && encoderIsTurning == 0 && rotaryStateCLK == HIGH) {
    rotaryStateCLK = LOW;
    encoderIsTurning = 1;
    encoderRight();
  } else if (rotaryStateCLK == LOW && encoderPossiblyBetween == false){
    rotaryStateCLK = HIGH; 
    encoderIsTurning = 0;
  }
}

void checkPotStates() {
  brightness = (analogRead(BRIGHTNESS_POT_PIN) / 1023.0);
  // Saturation is reversed
  saturation = 1.0 - (analogRead(SATURATION_POT_PIN) / 1023.0);
  //logBrightAndSat();
}

void logBrightAndSat() {
  Serial.print("brightness value: ");
  Serial.print(brightness);
  Serial.print(" saturation value: ");
  Serial.print(saturation);
  Serial.print("\n");
}

// triggered when encoder has been pressed
void encoderPress() {
  
}

// triggered when encoder has been turned left by one click
void encoderLeft() {
  setNewColor(-10);
}

// triggered when encoder has been turned right by one click
void encoderRight() {
  setNewColor(10);
}

// adjust hue by offset
void setNewColor(int offset) {
  hue += offset;
  while (hue > 360) {
    hue -= 360;  
  }
  while (hue < 0) {
    hue += 360;
  }

  // convert from HSV controls to RGB outputs to LED pins
  setRGBFromHSV();
  analogWrite(RED_LED_PIN, redVal);
  analogWrite(GREEN_LED_PIN, greenVal);
  analogWrite(BLUE_LED_PIN, blueVal); 
}

/**
 * HSV to RGB converter CAUTION: works on the global color values
 * modified to work in Arduino-land :)
 * hue - 0 -360
 * saturation - 0 - 1
 * value (brightness) - 0 - 1
 */
int setRGBFromHSV() {
  double c = (brightness * saturation);
  double hp = fmod(hue / 60.0, 6);
  double x = c * (1.0 - fabs(fmod(hp, 2.0) - 1.0));
  double m = brightness - c;

  // reset colors
  redVal = 0;
  greenVal = 0;
  blueVal = 0;
  
  if (hue >= 0 && hue < 60) {
    redVal = (int) 255.0 * (c + m);
    greenVal = (int) 255.0 * (x + m);
    blueVal = (int) 255.0 * m;
  } else if (hue >= 60 && hue < 120) {
    redVal = (int) 255.0 * (x + m);
    greenVal = (int) 255.0 * (c + m);
    blueVal = (int) 255.0 * m;
  } else if (hue >= 120 && hue < 180) {
    redVal = (int) 255.0 * m;
    greenVal = (int) 255.0 * (c + m);
    blueVal = (int) 255.0 * (x + m);
  } else if (hue >= 180 && hue < 240) {
    redVal = (int) 255.0 * m;
    greenVal = (int) 255.0 * (x + m);
    blueVal = (int) 255.0 * (c + m);
  } else if (hue >= 240 && hue < 300) {
    redVal = (int) 255.0 * (x + m);
    greenVal = (int) 255.0 * m;
    blueVal = (int) 255.0 * (c + m);
  } else if (hue >= 300 && hue <= 360) {
    redVal = (int) 255.0 * (c + m);
    greenVal = (int) 255.0 * m;
    blueVal = (int) 255.0 * (x + m);
  } else {
    // something terrible has occurred, bad input hue
    Serial.println("bad hue input to rgb converter");
  }

  //logColorState(c, x, m);
}

void logColorState(double c, double x, double m) {
  Serial.print("Hue: ");
  Serial.print(hue);
  Serial.print(" red: ");
  Serial.print(redVal);
  Serial.print(" green: ");
  Serial.print(greenVal);
  Serial.print(" blue: ");
  Serial.print(blueVal);
  Serial.print(" c: ");
  Serial.print(c);
  Serial.print(" x: ");
  Serial.print(x);
  Serial.print(" m: ");
  Serial.print(m);
  Serial.print("\n");
}

