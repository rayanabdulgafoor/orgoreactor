#include <OneWire.h>
#include <DallasTemperature.h>

#define frequency 5000
#define resolution 8
#define channel_1 0
#define channel_2 1
#define channel_3 2

const byte tempPin_1 = 2;
const byte tempPin_2 = 3;
const byte tempPin_3 = 4;

const byte temp1_out = 12;
const byte temp2_out = 13;
const byte temp3_out = 14;



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


  ledcSetup(channel_1, frequency, resolution);
  ledcSetup(channel_2, frequency, resolution);
  ledcSetup(channel_3, frequency, resolution);

  ledcAttachPin(temp1_out, channel_1);
  ledcAttachPin(temp2_out, channel_2);
  ledcAttachPin(temp3_out, channel_3);
}

void loop(void)
{
    tempSensor_1.requestTemperatures();
    tempSensor_2.requestTemperatures();
    tempSensor_3.requestTemperatures();

    temp[0] = tempSensor_1.getTempCByIndex(0);
    ledcWrite(channel_1, temp[0]);
    Serial.print(temp[0]);

    temp[1] = tempSensor_2.getTempCByIndex(0);
    ledcWrite(channel_2, temp[1]);
    Serial.print(temp[1]);

    temp[2] = tempSensor_3.getTempCByIndex(0);
    ledcWrite(channel_3, temp[2]);
    Serial.print(temp[2]);
}
