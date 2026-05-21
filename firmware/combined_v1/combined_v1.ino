#include <Arduino.h>
#include <Nextion.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <SPI.h>
#include <EEPROM.h>

// Variables and constants
const byte ledPin = 13;
const byte pH_enb_Pin = 12;
const byte temp_enb_pin = 10;
const byte alarmPin = 42;

// tempPage
// |-----this is to be defined in ESP---------------------|
// |const byte R1_tempPin = 2; // Input pin of temp sensor|
// |const byte R2_tempPin = 3; // Input pin of temp sensor|
// |const byte R3_tempPin = 4; // Input pin of temp sensor|
// |------------------------------------------------------|

//phPage
const byte R1_phPin = A0; // Input pin of pH sensor
const byte R2_phPin = A1; // Input pin of pH sensor
const byte R3_phPin = A2; // Input pin of pH sensor

// homePage
byte pumpBt_StatusChange = 0;

unsigned int R1_temp_val = 0; // value displayed on nextion
unsigned int R2_temp_val = 0; // value displayed on nextion
unsigned int R3_temp_val = 0; // value displayed on nextion

uint16_t R1_temp_upper_limit; // storing temp range
uint16_t R1_temp_lower_limit; // storing temp range
uint16_t R2_temp_upper_limit; // storing temp range
uint16_t R2_temp_lower_limit; // storing temp range
uint16_t R3_temp_upper_limit; // storing temp range
uint16_t R3_temp_lower_limit; // storing temp range

char buffer_R1_tU[100] = {0}; // recieving temp data from arduino
char buffer_R1_tL[100] = {0}; // recieving temp data from arduino
char buffer_R2_tU[100] = {0}; // recieving temp data from arduino
char buffer_R2_tL[100] = {0}; // recieving temp data from arduino
char buffer_R3_tU[100] = {0}; // recieving temp data from arduino
char buffer_R3_tL[100] = {0}; // recieving temp data from arduino

// TsetLimit page
bool R1_Talarm_status = false; // alarm status of temp for each reactor
bool R2_Talarm_status = false; // alarm status of temp for each reactor
bool R3_Talarm_status = false; // alarm status of temp for each reactor

// phPage

float R1_pH_val; // pH values in float format
float R2_pH_val; // pH values in float format
float R3_pH_val; // pH values in float format
// float calibration_value = 21.34; // calibration value

char R1_ph_buffer[10] = {0}; // pH values in char format to be send to nextion
char R2_ph_buffer[10] = {0}; // pH values in char format to be send to nextion
char R3_ph_buffer[10] = {0}; // pH values in char format to be send to nextion

float R1_ph_upper_limit; // storing temp range
float R1_ph_lower_limit; // storing temp range
float R2_ph_upper_limit; // storing temp range
float R2_ph_lower_limit; // storing temp range
float R3_ph_upper_limit; // storing temp range
float R3_ph_lower_limit; // storing temp range

int R1pHUpper = 0;
int R1pHlower = 0;
int R2pHUpper = 0;
int R2pHlower = 0;
int R3pHUpper = 0;
int R3pHlower = 0;

char buffer_R1_pU[100] = {0}; // receiving pH data from arduino
char buffer_R1_pL[100] = {0}; // receiving pH data from arduino
char buffer_R2_pU[100] = {0}; // receiving pH data from arduino
char buffer_R2_pL[100] = {0}; // receiving pH data from arduino
char buffer_R3_pU[100] = {0}; // receiving pH data from arduino
char buffer_R3_pL[100] = {0}; // receiving pH data from arduino

// PsetLimit page
bool R1_Palarm_status = false; // alarm status of pH for each reactor
bool R2_Palarm_status = false; // alarm status of pH for each reactor
bool R3_Palarm_status = false; // alarm status of pH for each reactor

// pumpPage
const byte pump_1_pin = 5; // output to pump
const byte pump_2_pin = 4; // output to pump
const byte pump_3_pin = 3; // output to pump
bool pump_1_enb = false;
bool pump_2_enb = false;
bool pump_3_enb = false;
byte setTbt_StatusChange = 0;
byte pump1_StatusChange = 0;
byte pump2_StatusChange = 0;
byte pump3_StatusChange = 0;

uint32_t set_status = 0;   // button status
uint32_t reset_status = 1; // button status
uint32_t hourVal = 0;      // input from timer
uint32_t minVal = 0;       // input from timer
uint32_t secVal = 0;       // input from timer

unsigned long time_in_ms = 0;  // converted time value in milliseconds
unsigned long currentTime = 0; // for millis()
unsigned long previousTime = 0;
unsigned long set_Time = 0; // time of setting timer

int pumpSatus = LOW; // pump status for checking conditions
byte alarm_Status = 0;

// alarmPage
int previous_page = 1;

int currentPage = 0;

// Function declaration:
//----------homePage----------------
void ligthBtPopCallback(void *ptr);
void phBtPopCallback(void *ptr);
void pumpBtPopCallback(void *ptr);
void tempBtPopCallback(void *ptr);

void saveTempBtPopCallback(void *ptr);
void savePhBtPopCallback(void *ptr);
//----------pumpPage-----------------
void pumpBt_1PopCallback(void *ptr);
void pumpBt_2PopCallback(void *ptr);
void pumpBt_3PopCallback(void *ptr);
void setTimerBtPopCallback(void *ptr);
void resetTimerBtPopCallback(void *ptr);
void alarmOffBtPopCallback(void *ptr);

//----------pages
void introPushCallback(void *ptr);
void homePagePushCallback(void *ptr);
void tempPagePushCallback(void *ptr);
void TsetLimitePushCallback(void *ptr);
void pumpPagePushCallback(void *ptr);
void phPagePushCallback(void *ptr);
void PsetLimitePushCallback(void *ptr);
void setPagePushCallback(void *ptr);
void alarmPagePushCallback(void *ptr);

// Decalring Objectss
//------------------intro------------------

NexVariable R1Tu = NexVariable(0, 4, "R1Tu"); // temp range values from EEPROM
NexVariable R1Tl = NexVariable(0, 5, "R1Tl"); // temp range values from EEPROM
NexVariable R2Tu = NexVariable(0, 6, "R2Tu"); // temp range values from EEPROM
NexVariable R2Tl = NexVariable(0, 7, "R2Tl"); // temp range values from EEPROM
NexVariable R3Tu = NexVariable(0, 8, "R3Tu"); // temp range values from EEPROM
NexVariable R3Tl = NexVariable(0, 9, "R3Tl"); // temp range values from EEPROM

// ph in int formate
NexVariable R1Pu = NexVariable(0, 10, "R1Pu"); // pH range values from EEPROM
NexVariable R1Pl = NexVariable(0, 11, "R1Pl"); // pH range values from EEPROM
NexVariable R2Pu = NexVariable(0, 12, "R2Pu"); // pH range values from EEPROM
NexVariable R2Pl = NexVariable(0, 13, "R2Pl"); // pH range values from EEPROM
NexVariable R3Pu = NexVariable(0, 14, "R3Pu"); // pH range values from EEPROM
NexVariable R3Pl = NexVariable(0, 15, "R3Pl"); // pH range values from EEPROM

//-----------------homePage----------------
NexDSButton lightBt = NexDSButton(1, 1, "lightBt");
NexDSButton phBt = NexDSButton(1, 2, "phBt");
NexDSButton pumpBt = NexDSButton(1, 3, "pumpBt");
NexDSButton tempBt = NexDSButton(1, 4, "tempBt");

//-----------------tempPage------------------
NexText R1_tU = NexText(2, 9, "R1_tU");                // temp range values from nextion
NexText R1_tL = NexText(2, 10, "R1_tL");               // temp range values from nextion
NexText R2_tU = NexText(2, 11, "R2_tU");               // temp range values from nextion
NexText R2_tL = NexText(2, 12, "R2_tL");               // temp range values from nextion
NexText R3_tU = NexText(2, 13, "R3_tU");               // temp range values from nextion
NexText R3_tL = NexText(2, 14, "R3_tL");               // temp range values from nextion
NexButton saveTempBt = NexButton(2, 22, "saveTempBt"); // save temp range in EEPROM

//----------------TsetLimit--------------------
NexVariable R1_tAl = NexVariable(2, 19, "R1_tAl"); // alarm button values
NexVariable R2_tAl = NexVariable(2, 20, "R2_tAl"); // alarm button values
NexVariable R3_tAl = NexVariable(2, 21, "R3_tAl"); // alarm button values
// NexDSButton tAlarm = NexDSButton(3, 2, "tAlarm");  // alarm button

//----------------phPage--------------------------
NexText R1_pH = NexText(4, 6, "R1_pH"); // pH value to displayed on nextion
NexText R2_pH = NexText(4, 7, "R2_pH"); // pH value to displayed on nextion
NexText R3_pH = NexText(4, 8, "R3_pH"); // pH value to displayed on nextion

NexVariable R1valU = NexVariable(4, 19, "R1valU"); // pH range values from nextion
NexVariable R1valL = NexVariable(4, 20, "R1valL"); // pH range values from nextion
NexVariable R2valU = NexVariable(4, 21, "R2valU"); // pH range values from nextion
NexVariable R2valL = NexVariable(4, 22, "R2valL"); // pH range values from nextion
NexVariable R3valU = NexVariable(4, 23, "R3valU"); // pH range values from nextion
NexVariable R3valL = NexVariable(4, 24, "R3valL"); // pH range values from nextion
NexButton savePhBt = NexButton(4, 28, "savePhBt"); // save PH range in EEPROM
//---------------PsetLimit------------------------
NexVariable R1_pAl = NexVariable(4, 25, "R1_pAl"); // alarm button values
NexVariable R2_pAl = NexVariable(4, 26, "R2_pAl"); // alarm button values
NexVariable R3_pAl = NexVariable(4, 27, "R3_pAl"); // alarm button values

//--------------pumpPage--------------------------
NexDSButton pumpBt_1 = NexDSButton(6, 10, "pumpBt_1"); // valve buttons
NexDSButton pumpBt_2 = NexDSButton(6, 11, "pumpBt_2"); // valve buttons
NexDSButton pumpBt_3 = NexDSButton(6, 12, "pumpBt_3"); // valve buttons

NexDSButton setTimerBt = NexDSButton(6, 13, "setTimerBt");     // set timer button
NexDSButton resetTimerBt = NexDSButton(6, 15, "resetTimerBt"); // reset button

NexNumber hourNum = NexNumber(6, 7, "hourNum"); // timer values from nextion
NexNumber minNum = NexNumber(6, 8, "minNum");   // timer values from nextion
NexNumber secNum = NexNumber(6, 9, "secNum");   // timer values from nextion

//---------------alarmPage-----------------------
NexPicture R1_p0 = NexPicture(8, 1, "R1_p0"); // alarm status icon
NexPicture R1_p1 = NexPicture(8, 2, "R1_p1"); // alarm status icon
NexPicture R2_p2 = NexPicture(8, 3, "R2_p2"); // alarm status icon
NexPicture R2_p3 = NexPicture(8, 4, "R2_p3"); // alarm status icon
NexPicture R3_p4 = NexPicture(8, 5, "R3_p4"); // alarm status icon
NexPicture R3_p5 = NexPicture(8, 6, "R3_p5"); // alarm status icon

NexButton alarmOffBt = NexButton(8, 7, "alarmOffBt");

//--------------pages------------------------------
NexPage intro = NexPage(0, 0, "intro");
NexPage homePage = NexPage(1, 0, "homePage");
NexPage tempPage = NexPage(2, 0, "tempPage");
NexPage TsetLimit = NexPage(3, 0, "TsetLimit");
NexPage phPage = NexPage(4, 0, "phPage");
NexPage PsetLimit = NexPage(5, 0, "PsetLimit");
NexPage pumpPage = NexPage(6, 0, "pumpPage");
NexPage setPage = NexPage(7, 0, "setPage");
NexPage alarmPage = NexPage(8, 0, "alarmPage");

NexTouch *nex_listen_list[] =
    {
        &lightBt,
        &phBt,
        &pumpBt,
        &tempBt,

        &saveTempBt,
        &savePhBt,

        &pumpBt_1,
        &pumpBt_2,
        &pumpBt_3,
        &setTimerBt,
        &resetTimerBt,

        &alarmOffBt,

        &intro,
        &homePage,
        &tempPage,
        &TsetLimit,
        &phPage,
        &PsetLimit,
        &pumpPage,
        &setPage,
        &alarmPage,
        NULL};

void ligthBtPopCallback(void *ptr)
{
  uint32_t lightEnb = 0;
  lightBt.getValue(&lightEnb);

  if (lightEnb == 1)
  {
    digitalWrite(ledPin, HIGH);
  }
  else
  {
    digitalWrite(ledPin, LOW);
  }
}
void phBtPopCallback(void *ptr)
{
  uint32_t phEnb = 0;
  phBt.getValue(&phEnb);

  if (phEnb == 1)
  {
    digitalWrite(pH_enb_Pin, HIGH);
  }
  else
  {
    digitalWrite(pH_enb_Pin, LOW);
  }
}
void pumpBtPopCallback(void *ptr)
{
  uint32_t pumpEnb = 0;
  pumpBt.getValue(&pumpEnb);

  if (pumpEnb == 1)
  {
    pumpSatus = HIGH;
    pump_1_enb = true;
    pump_2_enb = true;
    pump_3_enb = true;
  }
  else
  {
    // set_status = 0;
    pumpSatus = LOW;
    pump_1_enb = false;
    pump_2_enb = false;
    pump_3_enb = false;
  }
}
void tempBtPopCallback(void *ptr)
{
  uint32_t tempEnb = 0;
  tempBt.getValue(&tempEnb);

  if (tempEnb == 1)
  {
    digitalWrite(temp_enb_pin, HIGH);
  }
  else
  {
    digitalWrite(temp_enb_pin, LOW);
  }
}

void saveTempBtPopCallback(void *ptr)
{
  EEPROM.put(0, R1_temp_upper_limit);
  EEPROM.put(20, R1_temp_lower_limit);
  EEPROM.put(40, R2_temp_upper_limit);
  EEPROM.put(60, R2_temp_lower_limit);
  EEPROM.put(80, R3_temp_upper_limit);
  EEPROM.put(100, R3_temp_lower_limit);
}
void savePhBtPopCallback(void *ptr)
{
  EEPROM.put(120, R1pHUpper);
  EEPROM.put(140, R1pHlower);
  EEPROM.put(160, R2pHUpper);
  EEPROM.put(180, R2pHlower);
  EEPROM.put(200, R3pHUpper);
  EEPROM.put(220, R3pHlower);
}
// pumpPage
void pumpBt_1PopCallback(void *ptr)
{
  uint32_t pump_1 = 0;
  pumpBt_1.getValue(&pump_1);

  if (pump_1 == 1)
  {
    pump_1_enb = true;
  }
  else
  {
    pump_1_enb = false;
  }
}
void pumpBt_2PopCallback(void *ptr)
{
  uint32_t pump_2 = 0;
  pumpBt_2.getValue(&pump_2);

  if (pump_2 == 1)
  {
    pump_2_enb = true;
  }
  else
  {
    pump_2_enb = false;
  }
}
void pumpBt_3PopCallback(void *ptr)
{
  uint32_t pump_3 = 0;
  pumpBt_3.getValue(&pump_3);

  if (pump_3 == 1)
  {
    pump_3_enb = true;
  }
  else
  {
    pump_3_enb = false;
  }
}
void setTimerBtPopCallback(void *ptr)
{
  setTimerBt.getValue(&set_status);
  dbSerialPrintln(set_status);
  if (set_status == 1)
  {
    hourNum.getValue(&hourVal);
    minNum.getValue(&minVal);
    secNum.getValue(&secVal);
    set_Time = currentTime;
  }
}
void resetTimerBtPopCallback(void *ptr)
{
  resetTimerBt.getValue(&reset_status);
  if (reset_status == 0)
  {
    hourNum.getValue(&hourVal);
    minNum.getValue(&minVal);
    secNum.getValue(&secVal);
  }
}

void alarmOffBtPopCallback(void *ptr)
{
  //---------------temp-------------------------
  alarm_Status = 0;
  R1_p0.setPic(91);
  R1_p1.setPic(91);
  R2_p2.setPic(91);
  R2_p3.setPic(91);
  R3_p4.setPic(91);
  R3_p5.setPic(91);
  if (R1_Talarm_status == true)
  {
    R1_Talarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if (R2_Talarm_status == true)
  {
    R2_Talarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if (R3_Talarm_status == true)
  {
    R3_Talarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  //-----------------pH----------------------------
  if (R1_Palarm_status == true)
  {
    R1_Palarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if (R2_Palarm_status == true)
  {
    R2_Palarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
  if (R3_Palarm_status == true)
  {
    R3_Palarm_status = false;
    Serial2.print("page homePage");
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}

// Pages
void introPushCallback(void *ptr)
{
  currentPage = 0;
}
void homePagePushCallback(void *ptr)
{
  currentPage = 1;
}
void tempPagePushCallback(void *ptr)
{
  currentPage = 2;
  dbSerialPrintln("tempPagePushCallback");

  // collecting data
  memset(buffer_R1_tU, 0, sizeof(buffer_R1_tU));
  R1_tU.getText(buffer_R1_tU, sizeof(buffer_R1_tU));
  R1_temp_upper_limit = atoi(buffer_R1_tU);
  if (R1_temp_upper_limit == 0)
  {
    memset(buffer_R1_tU, 0, sizeof(buffer_R1_tU));
    R1_tU.getText(buffer_R1_tU, sizeof(buffer_R1_tU));
    R1_temp_upper_limit = atoi(buffer_R1_tU);
  }
  if (R1_temp_upper_limit == 0)
  {
    memset(buffer_R1_tU, 0, sizeof(buffer_R1_tU));
    R1_tU.getText(buffer_R1_tU, sizeof(buffer_R1_tU));
    R1_temp_upper_limit = atoi(buffer_R1_tU);
  }
  //------------------------------------------------------------------
  memset(buffer_R1_tL, 0, sizeof(buffer_R1_tL));
  R1_tL.getText(buffer_R1_tL, sizeof(buffer_R1_tL));
  R1_temp_lower_limit = atoi(buffer_R1_tL);
  if (R1_temp_lower_limit == 0)
  {
    memset(buffer_R1_tL, 0, sizeof(buffer_R1_tL));
    R1_tL.getText(buffer_R1_tL, sizeof(buffer_R1_tL));
    R1_temp_lower_limit = atoi(buffer_R1_tL);
  }
  if (R1_temp_lower_limit == 0)
  {
    memset(buffer_R1_tL, 0, sizeof(buffer_R1_tL));
    R1_tL.getText(buffer_R1_tL, sizeof(buffer_R1_tL));
    R1_temp_lower_limit = atoi(buffer_R1_tL);
  }
  //------------------------------------------------------------------
  memset(buffer_R2_tU, 0, sizeof(buffer_R2_tU));
  R2_tU.getText(buffer_R2_tU, sizeof(buffer_R2_tU));
  R2_temp_upper_limit = atoi(buffer_R2_tU);
  if (R2_temp_upper_limit == 0)
  {
    memset(buffer_R2_tU, 0, sizeof(buffer_R2_tU));
    R2_tU.getText(buffer_R2_tU, sizeof(buffer_R2_tU));
    R2_temp_upper_limit = atoi(buffer_R2_tU);
  }
  if (R2_temp_upper_limit == 0)
  {
    memset(buffer_R2_tU, 0, sizeof(buffer_R2_tU));
    R2_tU.getText(buffer_R2_tU, sizeof(buffer_R2_tU));
    R2_temp_upper_limit = atoi(buffer_R2_tU);
  }
  //-------------------------------------------------------------------
  memset(buffer_R2_tL, 0, sizeof(buffer_R2_tL));
  R2_tL.getText(buffer_R2_tL, sizeof(buffer_R2_tL));
  R2_temp_lower_limit = atoi(buffer_R2_tL);
  if (R2_temp_lower_limit == 0)
  {
    memset(buffer_R2_tL, 0, sizeof(buffer_R2_tL));
    R2_tL.getText(buffer_R2_tL, sizeof(buffer_R2_tL));
    R2_temp_lower_limit = atoi(buffer_R2_tL);
  }
  if (R2_temp_lower_limit == 0)
  {
    memset(buffer_R2_tL, 0, sizeof(buffer_R2_tL));
    R2_tL.getText(buffer_R2_tL, sizeof(buffer_R2_tL));
    R2_temp_lower_limit = atoi(buffer_R2_tL);
  }
  //-------------------------------------------------------------------
  memset(buffer_R3_tU, 0, sizeof(buffer_R3_tU));
  R3_tU.getText(buffer_R3_tU, sizeof(buffer_R3_tU));
  R3_temp_upper_limit = atoi(buffer_R3_tU);
  if (R3_temp_upper_limit == 0)
  {
    memset(buffer_R3_tU, 0, sizeof(buffer_R3_tU));
    R3_tU.getText(buffer_R3_tU, sizeof(buffer_R3_tU));
    R3_temp_upper_limit = atoi(buffer_R3_tU);
  }
  if (R3_temp_upper_limit == 0)
  {
    memset(buffer_R3_tU, 0, sizeof(buffer_R3_tU));
    R3_tU.getText(buffer_R3_tU, sizeof(buffer_R3_tU));
    R3_temp_upper_limit = atoi(buffer_R3_tU);
  }
  //-------------------------------------------------------------------
  memset(buffer_R3_tL, 0, sizeof(buffer_R3_tL));
  R3_tL.getText(buffer_R3_tL, sizeof(buffer_R3_tL));
  R3_temp_lower_limit = atoi(buffer_R3_tL);
  if (R3_temp_lower_limit == 0)
  {
    memset(buffer_R3_tL, 0, sizeof(buffer_R3_tL));
    R3_tL.getText(buffer_R3_tL, sizeof(buffer_R3_tL));
    R3_temp_lower_limit = atoi(buffer_R3_tL);
  }
  if (R3_temp_lower_limit == 0)
  {
    memset(buffer_R3_tL, 0, sizeof(buffer_R3_tL));
    R3_tL.getText(buffer_R3_tL, sizeof(buffer_R3_tL));
    R3_temp_lower_limit = atoi(buffer_R3_tL);
  }
  //-------------------------------------------------------------------
  uint32_t R1_al_status = 0;
  R1_tAl.getValue(&R1_al_status);

  if (R1_al_status == 1)
  {
    R1_Talarm_status = true;
  }
  else
  {
    R1_Talarm_status = false;
  }
  //-------------------------------------------------------------------
  uint32_t R2_al_status = 0;
  R2_tAl.getValue(&R2_al_status);

  if (R2_al_status == 1)
  {
    R2_Talarm_status = true;
  }
  else
  {
    R2_Talarm_status = false;
  }
  //-------------------------------------------------------------------
  uint32_t R3_al_status = 0;
  R3_tAl.getValue(&R3_al_status);

  if (R3_al_status == 1)
  {
    R3_Talarm_status = true;
  }
  else
  {
    R3_Talarm_status = false;
  }
  //-------------------------------------------------------------------
}

void TsetLimitePushCallback(void *ptr)
{
  currentPage = 3;
}

void phPagePushCallback(void *ptr)
{
  currentPage = 4;
  dbSerialPrintln("phPagePushCallback");

  // collecting data
  memset(buffer_R1_pU, 0, sizeof(buffer_R1_pU));
  R1valU.getText(buffer_R1_pU, sizeof(buffer_R1_pU));
  R1pHUpper = atoi(buffer_R1_pU);
  R1_ph_upper_limit = (float)atoi(buffer_R1_pU) * 0.01;
  if (R1_ph_upper_limit == 0)
  {
    memset(buffer_R1_pU, 0, sizeof(buffer_R1_pU));
    R1valU.getText(buffer_R1_pU, sizeof(buffer_R1_pU));
    R1pHUpper = atoi(buffer_R1_pU);
    R1_ph_upper_limit = (float)atoi(buffer_R1_pU) * 0.01;
  }
  if (R1_ph_upper_limit == 0)
  {
    memset(buffer_R1_pU, 0, sizeof(buffer_R1_pU));
    R1valU.getText(buffer_R1_pU, sizeof(buffer_R1_pU));
    R1pHUpper = atoi(buffer_R1_pU);
    R1_ph_upper_limit = (float)atoi(buffer_R1_pU) * 0.01;
  }
  //------------------------------------------------------------------
  memset(buffer_R1_pL, 0, sizeof(buffer_R1_pL));
  R1valL.getText(buffer_R1_pL, sizeof(buffer_R1_pL));
  R1pHlower = atoi(buffer_R1_pL);
  R1_ph_lower_limit = (float)atoi(buffer_R1_pL) * 0.01;
  if (R1_ph_lower_limit == 0)
  {
    memset(buffer_R1_pL, 0, sizeof(buffer_R1_pL));
    R1valL.getText(buffer_R1_pL, sizeof(buffer_R1_pL));
    R1pHlower = atoi(buffer_R1_pL);
    R1_ph_lower_limit = (float)atoi(buffer_R1_pL) * 0.01;
  }
  if (R1_ph_lower_limit == 0)
  {
    memset(buffer_R1_pL, 0, sizeof(buffer_R1_pL));
    R1valL.getText(buffer_R1_pL, sizeof(buffer_R1_pL));
    R1pHlower = atoi(buffer_R1_pL);
    R1_ph_lower_limit = (float)atoi(buffer_R1_pL) * 0.01;
  }
  //------------------------------------------------------------------
  memset(buffer_R2_pU, 0, sizeof(buffer_R2_pU));
  R2valU.getText(buffer_R2_pU, sizeof(buffer_R2_pU));
  R2pHUpper = atoi(buffer_R2_pU);
  R2_ph_upper_limit = (float)atoi(buffer_R2_pU) * 0.01;
  if (R2_ph_upper_limit == 0)
  {
    memset(buffer_R2_pU, 0, sizeof(buffer_R2_pU));
    R2valU.getText(buffer_R2_pU, sizeof(buffer_R2_pU));
    R2pHUpper = atoi(buffer_R2_pU);
    R2_ph_upper_limit = (float)atoi(buffer_R2_pU) * 0.01;
  }
  if (R2_ph_upper_limit == 0)
  {
    memset(buffer_R2_pU, 0, sizeof(buffer_R2_pU));
    R2valU.getText(buffer_R2_pU, sizeof(buffer_R2_pU));
    R2pHUpper = atoi(buffer_R2_pU);
    R2_ph_upper_limit = (float)atoi(buffer_R2_pU) * 0.01;
  }
  //-------------------------------------------------------------------
  memset(buffer_R2_pL, 0, sizeof(buffer_R2_pL));
  R2valL.getText(buffer_R2_pL, sizeof(buffer_R2_pL));
  R2pHlower = atoi(buffer_R2_pL);
  R2_ph_lower_limit = (float)atoi(buffer_R2_pL) * 0.01;
  if (R2_ph_lower_limit == 0)
  {
    memset(buffer_R2_pL, 0, sizeof(buffer_R2_pL));
    R2valL.getText(buffer_R2_pL, sizeof(buffer_R2_pL));
    R2pHlower = atoi(buffer_R2_pL);
    R2_ph_lower_limit = (float)atoi(buffer_R2_pL) * 0.01;
  }
  if (R2_ph_lower_limit == 0)
  {
    memset(buffer_R2_pL, 0, sizeof(buffer_R2_pL));
    R2valL.getText(buffer_R2_pL, sizeof(buffer_R2_pL));
    R2pHlower = atoi(buffer_R2_pL);
    R2_ph_lower_limit = (float)atoi(buffer_R2_pL) * 0.01;
  }
  //-------------------------------------------------------------------
  memset(buffer_R3_pU, 0, sizeof(buffer_R3_pU));
  R3valU.getText(buffer_R3_pU, sizeof(buffer_R3_pU));
  R3pHUpper = atoi(buffer_R3_pU);
  R3_ph_upper_limit = (float)atoi(buffer_R3_pU) * 0.01;
  if (R3_ph_upper_limit == 0)
  {
    memset(buffer_R3_pU, 0, sizeof(buffer_R3_pU));
    R3valU.getText(buffer_R3_pU, sizeof(buffer_R3_pU));
    R3pHUpper = atoi(buffer_R3_pU);
    R3_ph_upper_limit = (float)atoi(buffer_R3_pU) * 0.01;
  }
  if (R3_ph_upper_limit == 0)
  {
    memset(buffer_R3_pU, 0, sizeof(buffer_R3_pU));
    R3valU.getText(buffer_R3_pU, sizeof(buffer_R3_pU));
    R3pHUpper = atoi(buffer_R3_pU);
    R3_ph_upper_limit = (float)atoi(buffer_R3_pU) * 0.01;
  }
  //-------------------------------------------------------------------
  memset(buffer_R3_pL, 0, sizeof(buffer_R3_pL));
  R3valL.getText(buffer_R3_pL, sizeof(buffer_R3_pL));
  R3pHlower = atoi(buffer_R3_pL);
  R3_ph_lower_limit = (float)atoi(buffer_R3_pL) * 0.01;
  if (R3_ph_lower_limit == 0)
  {
    memset(buffer_R3_pL, 0, sizeof(buffer_R3_pL));
    R3valL.getText(buffer_R3_pL, sizeof(buffer_R3_pL));
    R3pHlower = atoi(buffer_R3_pL);
    R3_ph_lower_limit = (float)atoi(buffer_R3_pL) * 0.01;
  }
  if (R3_ph_lower_limit == 0)
  {
    memset(buffer_R3_pL, 0, sizeof(buffer_R3_pL));
    R3valL.getText(buffer_R3_pL, sizeof(buffer_R3_pL));
    R3pHlower = atoi(buffer_R3_pL);
    R3_ph_lower_limit = (float)atoi(buffer_R3_pL) * 0.01;
  }
  //-------------------------------------------------------------------
  uint32_t R1_Pal_status = 0;
  R1_pAl.getValue(&R1_Pal_status);

  if (R1_Pal_status == 1)
  {
    R1_Palarm_status = true;
  }
  else
  {
    R1_Palarm_status = false;
  }
  //-------------------------------------------------------------------
  uint32_t R2_Pal_status = 0;
  R2_pAl.getValue(&R2_Pal_status);

  if (R2_Pal_status == 1)
  {
    R2_Palarm_status = true;
  }
  else
  {
    R2_Palarm_status = false;
  }
  //-------------------------------------------------------------------
  uint32_t R3_Pal_status = 0;
  R3_pAl.getValue(&R3_Pal_status);

  if (R3_Pal_status == 1)
  {
    R3_Palarm_status = true;
  }
  else
  {
    R3_Palarm_status = false;
  }
  //-------------------------------------------------------------------
}

void PsetLimitePushCallback(void *ptr)
{
  currentPage = 5;
}
void pumpPagePushCallback(void *ptr)
{
  currentPage = 6;
}

void setPagePushCallback(void *ptr)
{
  currentPage = 7;
}

void alarmPagePushCallback(void *ptr)
{
  currentPage = 8;
}

void setTimer()
{
  unsigned long tm_in_ms = hourVal * 360000UL + minVal * 60000UL + secVal * 1000UL;
  if (currentTime <= (set_Time + tm_in_ms))
  {
    dbSerialPrintln("timer on");
    Serial.print(currentTime);
    Serial.print(" <---> ");
    Serial.print(set_Time);
    Serial.print(" <---> ");
    Serial.println(set_Time + tm_in_ms);
    pumpSatus = HIGH;
  }
  if (currentTime >= (set_Time + tm_in_ms))
  {
    dbSerialPrintln("timer off");
    setTbt_StatusChange = 1;
    pump1_StatusChange = 1;
    pump2_StatusChange = 1;
    pump3_StatusChange = 1;
    hourVal = minVal = secVal = 0;
    set_status = 0;
    pump_1_enb = false;
    pump_2_enb = false;
    pump_3_enb = false;
    pumpBt_StatusChange = 1;
  }
  // resetTimerBt.getValue(&reset_status);
  // if (reset_status == 0)
  // {
  //   dbSerialPrintln("timer off");
  //   set_status = 0;
  // }
}

void alarm()
{
  if (alarm_Status == 1)
    digitalWrite(alarmPin, HIGH);
  else
    digitalWrite(alarmPin, LOW);
}

void pumpOn()
{
  if (pump_1_enb == true)
    digitalWrite(pump_1_pin, HIGH);
  else
    digitalWrite(pump_1_pin, LOW);
  if (pump_2_enb == true)
    digitalWrite(pump_2_pin, HIGH);
  else
    digitalWrite(pump_2_pin, LOW);
  if (pump_3_enb == true)
    digitalWrite(pump_3_pin, HIGH);
  else
    digitalWrite(pump_3_pin, LOW);
}

void sendVal_tempPage()
{
  Serial2.print("R1_temp.val=");
  Serial2.print(R1_temp_val);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print("R2_temp.val=");
  Serial2.print(R2_temp_val);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  Serial2.print("R3_temp.val=");
  Serial2.print(R3_temp_val);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);

  dbSerialPrintln("R1:");
  dbSerialPrintln(R1_temp_upper_limit);
  dbSerialPrintln(R1_temp_lower_limit);
  dbSerialPrintln("R2:");
  dbSerialPrintln(R2_temp_upper_limit);
  dbSerialPrintln(R2_temp_lower_limit);
  dbSerialPrintln("R3:");
  dbSerialPrintln(R3_temp_upper_limit);
  dbSerialPrintln(R3_temp_lower_limit);
}

void sendVal_phPage()
{
  R1_pH.setText(R1_ph_buffer);
  R2_pH.setText(R2_ph_buffer);
  R3_pH.setText(R3_ph_buffer);

  dbSerialPrintln("R1:");
  dbSerialPrintln(R1_ph_upper_limit);
  dbSerialPrintln(R1_ph_lower_limit);
  dbSerialPrintln("R2:");
  dbSerialPrintln(R2_ph_upper_limit);
  dbSerialPrintln(R2_ph_lower_limit);
  dbSerialPrintln("R3:");
  dbSerialPrintln(R3_ph_upper_limit);
  dbSerialPrintln(R3_ph_lower_limit);
}

void measure_pH()
{
  int ph1_val = analogRead(R1_phPin);
  int ph2_val = analogRead(R2_phPin);
  int ph3_val = analogRead(R2_phPin);

  R1_pH_val = (float)ph1_val * 5.0 / 1024 / 6;
  R1_pH_val = 3.5 * R1_pH_val;
  // R1_pH_val = -5.70 * R1_pH_val + calibration_value;

  R2_pH_val = (float)ph2_val * 5.0 / 1024 / 6;
  R2_pH_val = 3.5 * R2_pH_val;
  // R2_pH_val = -5.70 * R2_pH_val + calibration_value;

  R3_pH_val = (float)ph3_val * 5.0 / 1024 / 6;
  R3_pH_val = 3.5 * R3_pH_val;
  // R3_pH_val = -5.70 * R3_pH_val + calibration_value;

  dtostrf(R1_pH_val, 3, 2, R1_ph_buffer);
  dtostrf(R2_pH_val, 3, 2, R2_ph_buffer);
  dtostrf(R3_pH_val, 3, 2, R3_ph_buffer);
}

// void measrue_temp()
// {
//   if (currentTime - previousTime_forTemp >= 1000UL)
//   {
//     tempSensors_R1.requestTemperatures();
//     tempSensors_R2.requestTemperatures();
//     tempSensors_R3.requestTemperatures();
//     R1_temp_val = tempSensors_R1.getTempCByIndex(0);
//     R2_temp_val = tempSensors_R2.getTempCByIndex(0);
//     R3_temp_val = tempSensors_R3.getTempCByIndex(0);
//     previousTime_forTemp = currentTime;
//   }
// }

void setup()
{
  //  Serial2.begin(9600);  // Start serial comunication at baud=9600
  //  delay(500);  // This dalay is just in case the nextion display didn't start yet, to be sure it will receive the following command.
  //  Serial2.print("baud=115200");  // Set new baud rate of nextion to 115200, but it's temporal. Next time nextion is power on,
  //  Serial2.write(0xff);  // We always have to send this three lines after each command sent to nextion.
  //  Serial2.write(0xff);
  //  Serial2.write(0xff);
  //  Serial2.end();  // End the serial comunication of baud=9600
  //  Serial2.begin(115200);  // Start serial comunication at baud=115200
  //  Serial.begin(115200);

  nexInit(); // Set the baudrate which is for debug and communicate with Nextion screen.

  // homePage
  pinMode(ledPin, OUTPUT);
  pinMode(pH_enb_Pin, OUTPUT);
  pinMode(temp_enb_pin, OUTPUT);

  // phPage
  pinMode(R2_phPin, INPUT);
  pinMode(R3_phPin, INPUT);
  pinMode(R1_phPin, INPUT);

  // pumpPage
  pinMode(pump_1_pin, OUTPUT);
  pinMode(pump_2_pin, OUTPUT);
  pinMode(pump_3_pin, OUTPUT);

  // Registering Events
  lightBt.attachPop(ligthBtPopCallback);
  phBt.attachPop(phBtPopCallback);
  pumpBt.attachPop(pumpBtPopCallback);
  tempBt.attachPop(tempBtPopCallback);

  saveTempBt.attachPop(saveTempBtPopCallback);
  savePhBt.attachPop(savePhBtPopCallback);

  pumpBt_1.attachPop(pumpBt_1PopCallback);
  pumpBt_2.attachPop(pumpBt_2PopCallback);
  pumpBt_3.attachPop(pumpBt_3PopCallback);

  setTimerBt.attachPop(setTimerBtPopCallback);
  resetTimerBt.attachPop(resetTimerBtPopCallback);

  alarmOffBt.attachPop(alarmOffBtPopCallback);

  intro.attachPush(introPushCallback);
  homePage.attachPush(homePagePushCallback);
  tempPage.attachPush(tempPagePushCallback);
  TsetLimit.attachPush(TsetLimitePushCallback);
  phPage.attachPush(phPagePushCallback);
  PsetLimit.attachPush(PsetLimitePushCallback);
  pumpPage.attachPush(pumpPagePushCallback);
  setPage.attachPush(setPagePushCallback);
  alarmPage.attachPush(alarmPagePushCallback);

  R1Tu.setValue(EEPROM.get(0, R1_temp_upper_limit));
  R1Tl.setValue(EEPROM.get(20, R1_temp_lower_limit));
  R2Tu.setValue(EEPROM.get(40, R2_temp_upper_limit));
  R2Tl.setValue(EEPROM.get(60, R2_temp_lower_limit));
  R3Tu.setValue(EEPROM.get(80, R3_temp_upper_limit));
  R3Tl.setValue(EEPROM.get(100, R3_temp_lower_limit));

  R1Pu.setValue(EEPROM.get(120, R1pHUpper));
  R1Pl.setValue(EEPROM.get(140, R1pHlower));
  R2Pu.setValue(EEPROM.get(160, R2pHUpper));
  R2Pl.setValue(EEPROM.get(180, R2pHlower));
  R3Pu.setValue(EEPROM.get(200, R3pHUpper));
  R3Pl.setValue(EEPROM.get(220, R3pHlower));

  dbSerialPrintln("setup done");
}

void loop()
{
  nexLoop(nex_listen_list);
  currentTime = millis();
  measure_pH();
  pumpOn();
  alarm();

  if (currentPage == 1)
  {
    if (pumpBt_StatusChange == 1)
    {
      pumpBt.setValue(0);
      pumpBt_StatusChange = 0;
    }
  }

  if (currentPage == 2)
  {
    sendVal_tempPage();
  }

  if (currentPage == 4)
  {
    sendVal_phPage();
  }

  if (set_status == 1)
  {
    setTimer();
  }
  if (currentPage == 6)
  {
    if (setTbt_StatusChange == 1)
    {
      setTimerBt.setValue(0);
      setTbt_StatusChange = 0;
    }
    if (pump1_StatusChange == 1)
    {
      setTimerBt.setValue(0);
      pump1_StatusChange = 0;
    }
    if (pump2_StatusChange == 1)
    {
      setTimerBt.setValue(0);
      pump2_StatusChange = 0;
    }
    if (pump3_StatusChange == 1)
    {
      setTimerBt.setValue(0);
      pump3_StatusChange = 0;
    }
    dbSerialPrint(hourVal + String("--"));
    dbSerialPrint(minVal + String("--"));
    dbSerialPrintln(secVal);
  }

  //----------------temp alarm--------------------------
  if (R1_Talarm_status == true)
  {
    if ((R1_temp_val > R1_temp_upper_limit || R1_temp_val < R1_temp_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R1_p0.setPic(90);
    }
  }
  else if (R2_Talarm_status == true)
  {
    if ((R2_temp_val > R2_temp_upper_limit || R2_temp_val < R2_temp_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R2_p2.setPic(90);
    }
  }
  else if (R3_Talarm_status == true)
  {
    if ((R3_temp_val > R3_temp_upper_limit || R3_temp_val < R3_temp_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R3_p4.setPic(90);
    }
  }
  //--------------pH alarm----------------------------
  if (R1_Palarm_status == true)
  {
    if ((R1_pH_val > R1_ph_upper_limit || R1_pH_val < R1_ph_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R1_p1.setPic(90);
    }
  }
  else if (R2_Palarm_status == true)
  {
    if ((R2_pH_val > R2_ph_upper_limit || R2_pH_val < R2_ph_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R2_p3.setPic(90);
    }
  }
  else if (R3_Palarm_status == true)
  {
    if ((R3_pH_val > R3_ph_upper_limit || R3_pH_val < R3_ph_lower_limit))
    {
      alarm_Status = 1;
      Serial2.print("page alarmPage");
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      R3_p5.setPic(90);
    }
  }

  delay(500);
}
