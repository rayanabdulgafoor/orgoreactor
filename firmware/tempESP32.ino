#include <OneWire.h>
#include <DallasTemperature.h>

const byte tempPin_1 = 2;
const byte tempPin_2 = 3;
const byte tempPin_3 = 4;

OneWire oneWire_ONE(tempPin_1);
OneWire oneWire_TWO(tempPin_2);
OneWire oneWire_THREE(tempPin_3);

DallasTemperature tempSensor_1(&oneWire_ONE);
DallasTemperature tempSensor_2(&oneWire_TWO);
DallasTemperature tempSensor_3(&oneWire_THREE);

int temp[3]= {};

void setup(void)
{
  Serial.begin(9600);
  tempSensor_1.begin(); 
  tempSensor_2.begin(); 
  tempSensor_3.begin(); 
}

void loop(void)
{
    tempSensor_1.requestTemperatures();
    tempSensor_2.requestTemperatures();
    tempSensor_3.requestTemperatures();

    temp[0] = tempSensor_1.getTempCByIndex(0);
    Serial.print(temp[0]);

    temp[1] = tempSensor_2.getTempCByIndex(0);
    Serial.print(temp[1]);

    temp[2] = tempSensor_3.getTempCByIndex(0);
    Serial.print(temp[2]);
}
