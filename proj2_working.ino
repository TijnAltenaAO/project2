// Define LDR pins (random gpio chosen, need to check first)
// base
#define LDR1 32
#define LDR2 33
#define LDR3 34
#define LDR4 35
#define LDR5 13
#define LDR6 12

// panel
#define LDR7 27

// motor pins
#define MOTOR_1_IN1 5
#define MOTOR_1_IN2 18
#define MOTOR_2_IN1 21
#define MOTOR_2_IN2 19
// motor sleep pin
#define MOTOR_SLEEP 25

// limit switches
#define baseLimitSwitch 22
#define upperLimitSwitch 4

// LEDs for showing battery voltages.
#define controlBatLED 16
#define controlBatRead 36
#define chargeBatLED 17
#define chargeBatRead 39

int middleLdrArr[3000];  // 1000 is arbitrary, depends on duration of check.
int ldrBaseArr[6];
int maxMiddleVal = 0;
int maxBaseVal = 0;
volatile int lastPressUpper = 0;
volatile int lastPressBase = 0;
int debounceTime = 5000;  // weird but works

volatile bool direction = true;
bool run = true;

TaskHandle_t myTaskHandle = NULL;


// task to flip Motor without blocking loop.
// void myTask(void *parameter) {
//   while (1) {
//     if (digitalRead(upperLimitSwitch) == HIGH && (millis() - lastPressUpper) > 3000) {  // check if should be HIGH, I think so.
//       Serial.println("LIMIT HIT");
//       lastPressUpper = millis();
//       Serial.print("before not gate: ");

//       Serial.println(direction);

//       direction = !direction;
//       Serial.println("after not gate: ");
//       Serial.println(direction);

//       stopMotor(180);
//       delay(1500);
//       startMotor(180, direction);
//     }

//     if (digitalRead(baseLimitSwitch) == LOW && (millis() - lastPressBase) > debounceTime) {
//       lastPressBase = millis();
//       direction = !direction;
//       stopMotor(360);
//       delay(1500);
//       startMotor(360, direction);
//     }

//     Serial.println(direction);

//     vTaskDelay(10 / portTICK_PERIOD_MS);  // Wait 1000 ms @todo put back to 10
//   }
// }

void resetPanelAngle() {
  Serial.println("entered reset panel function");
  // direction = true;
  startMotor(180, direction);
  Serial.println("motor started");
  while (digitalRead(upperLimitSwitch) == LOW) {
    // do nothing
    Serial.println("limit not hit");
    delay(10);
  }
  Serial.println("limit hit");
  stopMotor(180);
  Serial.println("stop motor");
  delay(200);
  startMotor(180, !direction);

  delay(1600);  // set to proper timing.
  stopMotor(180);
  delay(10);  // for motors sake might increase.
}

void setup() {
  Serial.begin(115200);

  // put your setup code here, to run once:
  pinMode(LDR1, INPUT);
  pinMode(LDR2, INPUT);
  pinMode(LDR3, INPUT);
  pinMode(LDR4, INPUT);
  pinMode(LDR5, INPUT);
  pinMode(LDR6, INPUT);
  pinMode(LDR7, INPUT);

  // motor pins
  pinMode(MOTOR_1_IN1, OUTPUT);
  pinMode(MOTOR_1_IN2, OUTPUT);
  pinMode(MOTOR_2_IN1, OUTPUT);
  pinMode(MOTOR_2_IN2, OUTPUT);
  pinMode(MOTOR_SLEEP, OUTPUT);

  // limit switch pins
  pinMode(baseLimitSwitch, INPUT_PULLUP);
  pinMode(upperLimitSwitch, INPUT_PULLUP);

  // LEDs
  pinMode(controlBatLED, OUTPUT);
  pinMode(chargeBatLED, OUTPUT);

  // battery pins
  pinMode(controlBatRead, INPUT);
  pinMode(chargeBatRead, INPUT);
}

void loop() {
  if (run == true) {
    // leds for batteries
    if (analogRead(controlBatRead) < 3100) {  // less than 3.5V
      digitalWrite(controlBatLED, HIGH);
    } else {
      digitalWrite(controlBatLED, LOW);
    }

    if (analogRead(chargeBatRead) > 3400) {  // change to 3700 or smth.
      digitalWrite(chargeBatLED, HIGH);
    } else {
      digitalWrite(chargeBatLED, LOW);
    }

    digitalWrite(MOTOR_SLEEP, HIGH);  // wake up driver
    delay(500);
    Serial.println("entering loop");
    resetPanelAngle();

    Serial.println("panel angle set. Continuing to 360.");


    run = false;
    return;
  }
  // if (run == true) {
  //   digitalWrite(MOTOR_SLEEP, HIGH);  // wake up driver

  //   delay(100);  // delay for serial monitor and driver.
  //   Serial.println("Entering loop");

  //   if (analogRead(controlBatRead) < 3100) {  // less than 3.5V
  //     digitalWrite(controlBatLED, HIGH);
  //   } else {
  //     digitalWrite(controlBatLED, LOW);
  //   }

  //   if (analogRead(chargeBatRead) > 3400) {  // change to 3700 or smth.
  //     digitalWrite(chargeBatLED, HIGH);
  //   } else {
  //     digitalWrite(chargeBatLED, LOW);
  //   }

  //   maxBaseVal = maxBaseLdr();
  //   resetPanelAngle();
  //   Serial.println("set panel angle");

  //   // Start a FreeRTOS task on Core 0 to monitor out limit switches.
  //   xTaskCreatePinnedToCore(
  //     myTask,         // Task function
  //     "MyTask",       // Name
  //     12000,          // Stack size (words) // morgen hoger proberen.
  //     NULL,           // Parameters
  //     1,              // Priority
  //     &myTaskHandle,  // Task handle
  //     0               // Run on core 0 (loop runs on core 1)
  //   );

  //   Serial.println("set task");

  //   delay(5000);

  //   // check if direction = 1 needed here.
  //   startMotor(360, direction);
  //   Serial.println("start motor 360");

  //   while (
  //     (readMiddleLdr() != (maxBaseVal - (maxBaseVal / 50))) && !(readMiddleLdr() > (maxBaseVal - (maxBaseVal / 50)))) {
  //     // Serial.println(direction);
  //   }

  //   if ((millis() - lastPressBase) < 500) {  // check if 500 suffices
  //     Serial.println("position is on limit, so we go further");

  //     delay(500);  // check if 500 suffices
  //   }

  //   stopMotor(360);  // add delay to be sure.
  //   Serial.println("found 360 position");
  //   delay(1500);
  //   Serial.println("start saving data.");
  //   maxMiddleVal = maxMiddleLdr();
  //   Serial.println("gathered enough data");
  //   delay(2000);

  //   Serial.println("start looking.");

  //   startMotor(180, direction);

  //   while (
  //     (readMiddleLdr() != (maxMiddleVal - (maxMiddleVal / 5))) && !(readMiddleLdr() > (maxMiddleVal - (maxMiddleVal / 5)))) {
  //     Serial.println("seeking");
  //   }

  //   if ((millis() - lastPressUpper) < 500) {  // check if 500 suffices
  //     Serial.println("position is on limit, so we go further");
  //     delay(500);  // check if 500 suffices and if rtos still triggers here.
  //   }

  //   Serial.println("found 180 position");

  //   stopMotor(180);
  //   delay(1500);  // for driver.

  //   digitalWrite(MOTOR_SLEEP, LOW);  // put driver to sleep.
  //   digitalWrite(chargeBatLED, LOW);
  //   digitalWrite(controlBatLED, LOW);
  //   run = false;
  //   delay(1500);  // for driver.
  // }
}


void startMotor(int motorId, bool direction) {
  if (motorId == 180) {
    if (direction == true) {
      digitalWrite(MOTOR_1_IN1, HIGH);
      digitalWrite(MOTOR_1_IN2, LOW);
    } else {
      digitalWrite(MOTOR_1_IN1, LOW);
      digitalWrite(MOTOR_1_IN2, HIGH);
    }
  } else {
    if (direction == true) {
      digitalWrite(MOTOR_2_IN1, HIGH);
      digitalWrite(MOTOR_2_IN2, LOW);
    } else {
      digitalWrite(MOTOR_2_IN1, LOW);
      digitalWrite(MOTOR_2_IN2, HIGH);
    }
  }
}

// todo remove param.
void stopMotor(int motorId) {
  digitalWrite(MOTOR_1_IN1, HIGH);
  digitalWrite(MOTOR_1_IN2, HIGH);
  digitalWrite(MOTOR_2_IN1, HIGH);
  digitalWrite(MOTOR_2_IN2, HIGH);
}

// function for reading all base ldr's, returning highest value
int maxBaseLdr() {
  int ldrBaseArr[6] = {
    analogRead(LDR1),
    analogRead(LDR2),
    analogRead(LDR3),
    analogRead(LDR4),
    analogRead(LDR5),
    analogRead(LDR6),
  };

  int size = sizeof(ldrBaseArr) / sizeof(ldrBaseArr[0]);

  return getMax(ldrBaseArr, size);
}

// function for reading middle panel ldr
int readMiddleLdr() {
  return analogRead(LDR7) * 1.5;
}

// function for reading max panel ldr
int maxMiddleLdr() {
  int duration = 2000;  // ik zeg maar wat.
  // direction = 1; // propably dumb. check at home why. it was int direction = 1 first. maybe check if direction = 1 works.
  startMotor(180, direction);

  for (int i = 0; i < duration; i++) {
    middleLdrArr[i] = readMiddleLdr();
    Serial.println(direction);
    delay(10);  // blocks rtos ?
  }

  stopMotor(180);
  Serial.println("stopped saving data");
  delay(2000);  // yusu
  int size = sizeof(middleLdrArr) / sizeof(middleLdrArr[0]);

  return getMax(middleLdrArr, size) * 1.5;
}

int getMax(int arr[], int size) {
  int maxVal = arr[0];
  for (int i = 1; i < size; i++) {
    if (arr[i] > maxVal) {
      maxVal = arr[i];
    }
  }
  return maxVal;
}
