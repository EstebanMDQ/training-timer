/*
 * Temporizador de entrenamiento
 * 
 * 
 */

#define DEBUG 1

// pines led status
const int ledStatus1 = 2;
const int ledStatus2 = 3;
int currLedStatus1 = LOW;
int currLedStatus2 = LOW;
long lastStatus1Flash;
long lastStatus2Flash;


// pines led tiempo
const int ledTime[5] = {6, 7, 8, 9, 5};
const int ledRest[2] = {12, 10};

// pin buzzer
const int buzzer = 11;

// button
const int pinBtn = 4;
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
int lastBtnState;
int currBtnState;
long holdBtnTimer = 0;
long holdBtnLimit = 1000;

// tiempos y pausas
const int times[5] = {60, 120, 180, 240, 360};
const int rest[2] = {30, 60};
//const int times[3] = {6, 12, 18};
//const int rest[2] = {3, 6};
int currWorkTime = 0;
int currRest = 1;


// variable to control mode
// HIGH program
// LOW timer running
int mode = HIGH; 
int timerOn = LOW; // is the timer running
int working = HIGH; // workingout or resting
long nextTimerChange = 0; // when will the timer change next


void loop() {
  long currTime = millis();
  int btnRead = digitalRead(pinBtn);

  // mantener el boton para cambiar de modo
  
  if (currTime > lastDebounceTime + debounceDelay && btnRead != lastBtnState) {
    lastDebounceTime = currTime;
    if (btnRead == HIGH && currBtnState == LOW) {
      holdBtnTimer = currTime;
      currBtnState = HIGH;
    #ifdef DEBUG
      Serial.println("comenzo holdBtnTimer");
    #endif
    }
    if (btnRead == LOW && currBtnState == HIGH) {
      if (currTime - holdBtnTimer > holdBtnLimit) {
        mode = !mode;
        if (mode) {
          working = LOW;
        }
      #ifdef DEBUG
        Serial.print("finalizo holdBtnTimer ");
        Serial.println(currTime - holdBtnTimer);
      #endif
      } else {
        if (mode == HIGH) {
          currWorkTime++;
          if (currWorkTime == 5) {
            currWorkTime = 0;
            currRest++;
          }
          if (currRest == 2) 
            currRest = 0;
          toggleLedTime(ledTime[currWorkTime], ledRest[currRest]);
        } else {
          timerOn = !timerOn;
          if (timerOn) {
            startWorking(currTime);
          } else {
            stopWorking(currTime);
          }
        }
        dumpStatus();
      }
    }
    currBtnState = btnRead;    
  }

  lastBtnState = btnRead;
  
  if (mode) {
    if (currTime > lastStatus1Flash + 400) {
      currLedStatus1 = !currLedStatus1;
      digitalWrite(ledStatus1, currLedStatus1);
      lastStatus1Flash = currTime;
    }
    if (currTime > lastStatus2Flash + 300) {
      currLedStatus2 = !currLedStatus2;
      digitalWrite(ledStatus2, currLedStatus2);
      lastStatus2Flash = currTime;
    }
    
  } else {
    if (timerOn) {
      if (currTime > nextTimerChange) {
        dumpStatus();
        if (working) {
          stopWorking(currTime);
        } else {
          startWorking(currTime);
        }
      }
    } else {
      digitalWrite(ledStatus1, LOW);
      digitalWrite(ledStatus2, LOW);
    }
  }


//  if(currTime - lastDebounceTime) > debounceDelay) {
//
//    
//  }
}

void startWorking(long currTime) {
  toneUp();
  nextTimerChange = currTime + long(times[currWorkTime]) * 1000;
  digitalWrite(ledStatus1, HIGH);
  digitalWrite(ledStatus2, LOW);  
  working = HIGH;
}

void stopWorking(long currTime) {
  toneDown();
  nextTimerChange = currTime + long(rest[currRest]) * 1000;
  digitalWrite(ledStatus1, LOW);
  digitalWrite(ledStatus2, HIGH);
  working = LOW;
}

void dumpStatus() {
#ifdef DEBUG
  Serial.println("================");  
  Serial.print("currWorkTime = ");  
  Serial.println(currWorkTime);
  Serial.print("currRest = ");  
  Serial.println(currRest);
  Serial.print("mode = ");  
  Serial.println(mode);
  Serial.print("timerOn = ");  
  Serial.println(timerOn);
  Serial.print("working = ");  
  Serial.println(working);
  Serial.print("nextTimerChange = ");  
  Serial.println(nextTimerChange);
  Serial.print("currTime = ");
  Serial.println(millis());
  Serial.println("================");  
#endif
}

void toneUp() {
  digitalWrite(buzzer, LOW);
  delay(1000);
  digitalWrite(buzzer, HIGH);
}

void toneDown() {
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
  delay(300);
  digitalWrite(buzzer, LOW);
  delay(500);
  digitalWrite(buzzer, HIGH);
}


void setup() {
#ifdef DEBUG
  Serial.begin(9600);
#endif

  // inicializo buzzer
  pinMode(buzzer, OUTPUT);
//  digitalWrite(buzzer, HIGH);

  // inicializo leds status
  pinMode(ledStatus1, OUTPUT);
  pinMode(ledStatus2, OUTPUT);

  // inicializo leds tiempos
  pinMode(ledTime[0], OUTPUT);
  pinMode(ledTime[1], OUTPUT);
  pinMode(ledTime[2], OUTPUT);
  pinMode(ledTime[3], OUTPUT);
  pinMode(ledTime[4], OUTPUT);

  // inicializo pines descanzo
  pinMode(ledRest[0], OUTPUT);
  pinMode(ledRest[1], OUTPUT);

  // inicializo boton
  pinMode(pinBtn, INPUT);

//  digitalWrite(ledStatus1, HIGH);
//  digitalWrite(ledStatus2, LOW);
//  delay(30);
//  digitalWrite(ledStatus1, LOW);
//  digitalWrite(ledStatus2, HIGH);
//  delay(30);
//  digitalWrite(ledStatus2, LOW);
//  delay(30);
  digitalWrite(ledStatus1, HIGH);
  digitalWrite(ledStatus2, HIGH);
  delay(30);
  digitalWrite(ledStatus1, LOW);
  digitalWrite(ledStatus2, LOW);


  for( int i=0; i<5; i++) {
    toggleLedTime(ledTime[i], ledRest[i%2]);
    delay(50); 
  }

  toggleLedTime(ledTime[0], ledRest[1]);
  toneUp();
}

void toggleLedTime(int pin1, int pin2) {
  int i;
  for (i=0; i<5; i++) {
    digitalWrite(ledTime[i], (pin1 == ledTime[i]));
  }
  for (i=0; i<2; i++) {
    digitalWrite(ledRest[i], (pin2 == ledRest[i]));
  }
}

