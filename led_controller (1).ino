const int ledPin = 0;
const int sensor = 2;
const int buttonDown = 3;
const int buttonUp = 4;

bool previousState = HIGH;
bool isRunning = false;
float phase = 0.5;
unsigned long pressTime = 0;
unsigned long lastChange = 0;
unsigned long lastAnimationStep = 0;
bool waiting = false;
float maxBrightness = 125;
const float brightnessStep = 3;
const int changeInterval = 30;
const int animationInterval = 1;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(sensor, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
}

void loop() {
  bool state = digitalRead(sensor);
  bool down = digitalRead(buttonDown) == LOW;
  bool up = digitalRead(buttonUp) == LOW;

  if (millis() - lastChange >= changeInterval) {
    if (down) {
      maxBrightness -= brightnessStep;
      if (maxBrightness < 10) maxBrightness = 10;
      lastChange = millis();
    }
    if (up) {
      maxBrightness += brightnessStep;
      if (maxBrightness > 255) maxBrightness = 255;
      lastChange = millis();
    }
  }

  if (state == LOW && previousState == HIGH && !isRunning) {
    waiting = true;
    pressTime = millis();
  }

  if (state == HIGH && previousState == LOW) {
    waiting = false;
  }

  if (waiting && millis() - pressTime >= 150) {
    waiting = false;
    isRunning = true;
    phase = 0.5;
  }

  if (isRunning && millis() - lastAnimationStep >= animationInterval) {
    lastAnimationStep = millis();
    phase += 0.00055;
    float sine = sin(phase * PI) * 0.5 + 0.5;
    int pwm = (int)(pow(sine, 2.2) * (float)maxBrightness);
    analogWrite(ledPin, pwm);
    if (phase >= 2.5) {
      phase = 0.5;
      if (state == HIGH) {
        isRunning = false;
        analogWrite(ledPin, maxBrightness);
      }
    }
  } else if (!isRunning) {
    analogWrite(ledPin, maxBrightness);
  }

  previousState = state;
}
