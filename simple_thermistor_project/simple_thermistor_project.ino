using namespace std;
#include <math.h>

/* LED segment positions
 *    ┌─────┐
 *       A
 * ┌ ┐└─────┘┌ ┐
 * │ │       │ │
 * │F│       │B│
 * │ │       │ │
 * └ ┘┌─────┐└ ┘
 *       G
 * ┌ ┐└─────┘┌ ┐
 * │ │       │ │
 * │E│       │C│
 * │ │       │ │
 * └ ┘┌─────┐└ ┘┌──┐
 *       D       DP
 *    └─────┘   └──┘
 */
#define segA 9
#define segB 7
#define segC 3
#define segD 5
#define segE 6
#define segF 8
#define segG 2
#define segDP 4  // decimal point

// note: digits start on left; 2031 == {dig0 = 2, dig1 = 0, dig2 = 3, dig3 = 1}
// note: the digit pins on the LED display may start counting at one and not zero.
#define dig0 10
#define dig1 12
#define dig2 13
#define dig3 11

// pins for ntc thermistor
#define ntcInput A7
#define ntcPower A5

// thermistor variables
int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temperature;
char digits[4];
bool decimalPoints[4] = { false, false, false, false };

// display variables
String tempString;

// the value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 500;         // interval at which to update display

void setup() {
  pinMode(segA, OUTPUT);
  pinMode(segB, OUTPUT);
  pinMode(segC, OUTPUT);
  pinMode(segD, OUTPUT);
  pinMode(segE, OUTPUT);
  pinMode(segF, OUTPUT);
  pinMode(segG, OUTPUT);
  pinMode(segDP, OUTPUT);
  pinMode(dig0, OUTPUT);
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);

  pinMode(ntcPower, OUTPUT);     // being used as a digital power output pin.
  digitalWrite(ntcPower, HIGH);  // this allows power to be turned on and off.
  pinMode(ntcInput, INPUT);      // analog
}

void loop() {
  unsigned long currentMillis = millis();
  //update display ever interval(500) ms
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    R2 = R1 * (1023.0 / (float)(analogRead(ntcInput)) - 1.0);
    logR2 = log(R2);
    temperature = ((1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)) - 273.15);
    //rounds temperature to one decimal place.
    temperature = round(temperature * 10) / 10.0;

    // cast float to string, convert back to digit, store in array, loop over array, display digits
    tempString = String(temperature);

    if (temperature > 100) {
      // xxx.x
      digits[0] = tempString.charAt(0);
      digits[1] = tempString.charAt(1);
      digits[2] = tempString.charAt(2);
      digits[3] = tempString.charAt(4);
      setDecimalPoint(2);

    } else if (temperature < 100.0 && temperature >= 10.0) {
      // xx.xc
      digits[0] = tempString.charAt(0);
      digits[1] = tempString.charAt(1);
      digits[2] = tempString.charAt(3);
      digits[3] = 'c';
      setDecimalPoint(1);

    } else if (temperature < 10.0 && temperature > 0.0) {
      // x.xc
      digits[0] = '0';
      digits[1] = tempString.charAt(0);
      ;
      digits[2] = tempString.charAt(2);
      digits[3] = 'c';
      setDecimalPoint(1);

    } else if (temperature < 0.0 && temperature > -10.0) {
      // -x.xc
      digits[0] = '-';
      digits[1] = tempString.charAt(1);
      digits[2] = tempString.charAt(3);
      digits[3] = 'c';
      setDecimalPoint(1);

    } else if (temperature <= -10.0 && temperature > -100.0) {
      // -xx.x
      digits[0] = '-';
      digits[1] = tempString.charAt(1);
      digits[2] = tempString.charAt(2);
      digits[3] = tempString.charAt(4);
      setDecimalPoint(2);
    }
  }

  // this delay controls the brightness (0 for full brightness, 20 for dim)
  delay(5);


  // loops through display functions for each digit
  for (int i = 0; i < 4; i++) {
    // these functions are for common cathode 4 digit, 7 segment LED display
    clearSegments();
    selectDigit(i);
    displayNumber(digits[i]);
    displayDecimalPoint(decimalPoints[i]);
    delayMicroseconds(200);
    clearSegments();
  }
}

// determine which segments to set
void displayNumber(char x) {
  switch (x) {
    case '1':
      one();
      break;
    case '2':
      two();
      break;
    case '3':
      three();
      break;
    case '4':
      four();
      break;
    case '5':
      five();
      break;
    case '6':
      six();
      break;
    case '7':
      seven();
      break;
    case '8':
      eight();
      break;
    case '9':
      nine();
      break;
    case 'c':
      celcius();
      break;
    case '-':
      negative();
      break;
    default:
      zero();
      break;
  }
}

// display decimal point segment
void displayDecimalPoint(bool displayDecimalPoint) {
  if (displayDecimalPoint) {
    digitalWrite(segDP, HIGH);
  }
}

// set decimal array
void setDecimalPoint(int x) {
  decimalPoints[0] = false;
  decimalPoints[1] = false;
  decimalPoints[2] = false;
  decimalPoints[3] = false;

  decimalPoints[x] = true;
}

// open dx port by setting specified digit pin to low
void selectDigit(int x) {
  digitalWrite(dig0, HIGH);
  digitalWrite(dig1, HIGH);
  digitalWrite(dig2, HIGH);
  digitalWrite(dig3, HIGH);
  switch (x) {
    case 0:
      digitalWrite(dig0, LOW);
      break;
    case 1:
      digitalWrite(dig1, LOW);
      break;
    case 2:
      digitalWrite(dig2, LOW);
      break;
    default:
      digitalWrite(dig3, LOW);
      break;
  }
}

// Clear LED segments
void clearSegments() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segDP, LOW);
}

// set segments to display super script 'c'
void celcius() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segDP, LOW);
}

// set segments to display '-' symbol
void negative() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
  digitalWrite(segDP, LOW);
}

// set segments to display 0
void zero() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, LOW);
}

// set segments to display 1
void one() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}

// set segments to display 2
void two() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
}

// set segments to display 3
void three() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
}

// set segments to display 4
void four() {
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

// set segments to display 5
void five() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

// set segments to display 6
void six() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

// set segments to display 7
void seven() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}

// set segments to display 8
void eight() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

// set segments to display 9
void nine() {
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}