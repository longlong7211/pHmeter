
#include "FS.h"
#include "SPIFFS.h"
#define FORMAT_SPIFFS_IF_FAILED true


#include <OneWire.h>
#include <DallasTemperature.h>

#define LED 27
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#define pHSensor 34
#define pinButton 12
#define SensorPin 34
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth  40

#define bt digitalRead(pinButton)



int pHArray[ArrayLenth];
int pHArrayIndex = 0;

String of;
float Offset;


void setup() {
  Serial.begin(9600);
  sensors.begin();
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }
  delay(3000);

  of = readFile(SPIFFS, "/pH.txt");
  Offset = of.toFloat();
  Serial.println(Offset);
  //    deleteFile(SPIFFS, "/pH.txt");
}







void loop() {


  if (ReadButton() == 2) {
    unsigned long bling;
    int mod = 0;
    int timebling = 2000;

    while (ReadButton() != 2) {


      if (millis() - bling > timebling) {
        digitalWrite(LED, !digitalRead(LED));
        bling = millis();
      }


      if (ReadButton == 0) {
        mod++;
        if (mod == 1) {
          timebling = 1500;
        }
        if (mod == 2) {
          timebling = 1000;
        }
        if (mod == 3) {
          timebling = 500;
        }
      }



    }
    Calibration(mod);
  }
}





float ReadTemp() {


  sensors.requestTemperatures();
  float t = sensors.getTempCByIndex(0);
  Serial.print("Nhiet do");
  Serial.println(t);

  return t;
}







int ReadButton() {

  boolean check = false, smc = false, cali = false;
  unsigned long timePress;



  if (bt) {
    check = true;
  } else {


    if (check) {
      if (timePress > 8000) {

      }


      if (timePress > 5000) {

        //        calibration();
        check = false;
        timePress = millis();
        return 2;
      }



      if (timePress > 3000) {

        //smart coonfig
        check = false;
        timePress = millis();
        return 1;
      }



      if (timePress < 1000) {
        check = false;
        timePress = millis();
        return 0;
      }


    }

    timePress = millis();
    check = false;
  }
}




void Calibration(int modecali) {
  unsigned long timecali;
  float offsetcali;
  //mod 4.01pH
  if (modecali == 1) {
    timecali = millis();
    while (timecali < 30000) {
      offsetcali = ph();
    }
    offsetcali = 4.01 - offsetcali;
  }


  //mode 6.86pH
  if (modecali == 2) {
    timecali = millis();
    while (timecali < 30000) {
      offsetcali = ph();
    }
    offsetcali = 6.86 - offsetcali;
  }


  //mode 9.18pH
  if (modecali == 3) {
    timecali = millis();
    while (timecali < 30000) {
      offsetcali = ph();
    }
    offsetcali = 9.18 - offsetcali;
  }
  char w = offsetcali;
  Serial.println("calibration success!!! waiting 3 second...");
  deleteFile(SPIFFS, "/pH.txt");
  delay(1000);
  writeFile(SPIFFS, "/pH.txt", &w);
  delay(1000);
}






float ph() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;
  if (millis() - samplingTime > samplingInterval)
  {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth)pHArrayIndex = 0;
    voltage = avergearray(pHArray, ArrayLenth) * 4.75 / 4095;
    pHValue = 3.5 * voltage + Offset;
    samplingTime = millis();
  }
  if (millis() - printTime > printInterval)  //Every 800 milliseconds, print a numerical, convert the state of the LED indicator
  {
    Serial.print("Voltage:");
    Serial.print(voltage, 2);
    Serial.print("    pH value: ");
    Serial.println(pHValue, 2);
    printTime = millis();
  }
  return (pHValue, 2);
}










double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;      //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}
