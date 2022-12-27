using namespace std;

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

// note: digits start on left; 2031 == {dig1 = 2, dig2 = 0, dig3 = 3, dig4 = 1}
#define dig1 10
#define dig2 12
#define dig3 13
#define dig4 11

#define ntcInput A7
#define ntcPower A5

// thermistor variables

int Vo;
float R1 = 10000;
float logR2, R2, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float temperature;
int digits[4];

char dTens;
char dOnes;
char dtenths;
char dhundredths;

String tempString;

// The value will quickly become too large for an int to store
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
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(dig4, OUTPUT);

  pinMode(ntcPower, OUTPUT);
  digitalWrite(ntcPower, HIGH);
  pinMode(ntcInput, INPUT);  // analog


}

void loop() {
  unsigned long currentMillis = millis();
  //update display ever invertal(500) ms
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Vo = analogRead(ntcInput);
    R2 = R1 * (1023.0 / (float)Vo - 1.0);
    logR2 = log(R2);
    temperature = ((1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2)) - 273.15);

    // cast float to string, convert back to digit, store in array, loop over array, display digits
    tempString = String(temperature);
    if (temperature < 100.0 && temperature >= 10.0) {
      dTens = tempString.charAt(0);
      dOnes = tempString.charAt(1);
      dtenths = tempString.charAt(3);
      dhundredths = tempString.charAt(4);

      //convert char to int
      digits[0] = dTens - '0';
      digits[1] = dOnes - '0';
      digits[2] = dtenths - '0';
      digits[3] = dhundredths - '0';

      //round to one decimal point
      //ex: 39.96 => 40.0(0)
      if (digits[3] >= 5) {
        digits[3] = 0;
        digits[2]++;
      }
      if (digits[2] == 10) {
        digits[2] = 0;
        digits[1]++;
      }
      if (digits[1] == 10) {
        digits[1] = 0;
        digits[0]++;
      }

    } else if (temperature < 10.0 && temperature > 0) {
      dOnes = tempString.charAt(0);
      dtenths = tempString.charAt(2);
      dhundredths = tempString.charAt(3);

      //convert char to int
      digits[0] = 0;
      digits[1] = dOnes - '0';
      digits[2] = dtenths - '0';
      digits[3] = dhundredths - '0';
    }
    //TODO:write for negative temps
    //TODO:write code that displays c if digit is -99 or something
  }


  //this delay controls the brightness (0 for full brightness, 20 for dim)
  delay(5);
  
//TODO:write loop here for zero to three
  //display
  clearDisplay();         //Turn off all LED lights
  pickDigit(1);           //Selection of a digital display
  pickNumber(digits[0]);  //Display digital d1
  delayMicroseconds(200);

  clearDisplay();          //Turn off all LED lights
  pickDigit(2);            //Select the first two digital display
  displayDecimalPoint(2);  //Decimal display
  pickNumber(digits[1]);   //Display digital d2
  delayMicroseconds(200);

  clearDisplay();         //Turn off all LED lights
  pickDigit(3);           //Select the first three digital display
  pickNumber(digits[2]);  //Display digital d3
  delayMicroseconds(200);

  //set dig 4 to 'c'
  clearDisplay();
  pickDigit(4);
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
  digitalWrite(segDP, LOW);
  delayMicroseconds(200);
  clearDisplay();
}


void pickNumber(int x)  //Defined pickNumber (x), whose role is to display digital x
{
  switch (x) {
    case 1:
      one();
      break;
    case 2:
      two();
      break;
    case 3:
      three();
      break;
    case 4:
      four();
      break;
    case 5:
      five();
      break;
    case 6:
      six();
      break;
    case 7:
      seven();
      break;
    case 8:
      eight();
      break;
    case 9:
      nine();
      break;
    default:
      zero();
      break;
  }
}

void displayDecimalPoint(int x)  //Decimal point setting Open
{
  digitalWrite(segDP, HIGH);
}

void pickDigit(int x)  //Defined pickDigit (x), whose role is to open the port dx
{
  digitalWrite(dig1, HIGH);
  digitalWrite(dig2, HIGH);
  digitalWrite(dig3, HIGH);
  digitalWrite(dig4, HIGH);
  switch (x) {
    case 1:
      digitalWrite(dig1, LOW);
      break;
    case 2:
      digitalWrite(dig2, LOW);
      break;
    case 3:
      digitalWrite(dig3, LOW);
      break;
    default:
      digitalWrite(dig4, LOW);
      break;
  }
}

void clearDisplay()  // Clear LED screen
{
  digitalWrite(segA, LOW);
  digitalWrite(segB, LOW);
  digitalWrite(segC, LOW);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
  digitalWrite(segDP, LOW);
}



void zero()  //Define those figures 0 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, LOW);
}

void one()  //Define those figures 1 cathode pin switch
{
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}

void two()  //Define those figures 2 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, LOW);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
}

void three()  //Define those figures 3 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, HIGH);
}

void four()  //Define those figures 4 cathode pin switch
{
  digitalWrite(segA, LOW);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

void five()  //Define those figures 5 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

void six()  //Define those figures 6 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, LOW);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

void seven()  //Define those figures 7 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, LOW);
  digitalWrite(segE, LOW);
  digitalWrite(segF, LOW);
  digitalWrite(segG, LOW);
}

void eight()  //Define those figures 8 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, HIGH);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}

void nine()  //Define those figures 9 cathode pin switch
{
  digitalWrite(segA, HIGH);
  digitalWrite(segB, HIGH);
  digitalWrite(segC, HIGH);
  digitalWrite(segD, HIGH);
  digitalWrite(segE, LOW);
  digitalWrite(segF, HIGH);
  digitalWrite(segG, HIGH);
}