// Set pins
int RED_LED_PIN = 3;
int GREEN_LED_PIN = 10;
int BLUE_LED_PIN = 11;

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

void setup() {
  // put your setup code here, to run once:
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  // test rotating hue
  hue += 1;
  if (hue > 360) {
    hue = 0;  
  }

  // convert from HSV controls to RGB outputs to LED pins
  setRGBFromHSV();
  analogWrite(RED_LED_PIN, redVal);
  analogWrite(GREEN_LED_PIN, greenVal);
  analogWrite(BLUE_LED_PIN, blueVal);

  // Let's not get crazy now
  delay(50);
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

