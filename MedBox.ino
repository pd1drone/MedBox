
//potentio 
//pin1 5V
//pin2 A5
//pin3 GND

//LED
//MAHABANG PAA 13
//MALIIT NA PAA GND

//IR SENSOR
//Out DIGITAL 2
//GND GND
//VCC 5V

//BUZZER 
// positive 9
// negative GND

// SDCARD ADAPTER
// CS DIGITAL 4
// SCK DIGITAL PIN 52
// MOSI DIGITAL PIN 51
// MISO DIGITAL PIN 50

// GSM MODULE SIM800L
// RX = DIGITAL PIN 14 TX
// TX = DIGITAL PIN 15 RX
// VCC = +3.7 VOLTS on LM2596
// GND = GND ARDUINO AND GND LM2596

//PUSHBUTTON
// DIGITAL PIN 5
// GND

//SERVO MOTOR
//`ORANGE WIRE DIGITAL PIN 8
// RED WIRE = 5V Arduino
// BROWN WIRE = GND

//RTC MODULE
//SCL = DIGITAL PIN 21 SCL
//SDA = DIGITAL PIN 20 SDA
// VCC = 5V arduino
// GND = GND

// LCD 16x2
// SCL=DIGITAL PIN 21 SCL
// SDA = DIGITAL PIN 20 SDA
// VCC = 5V arduino
// GND = GND


#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "RTClib.h"
#include <Servo.h>
#include <SD.h>
#include <SPI.h>
#include "TimeLib.h"
#include <UnixTime.h>

RTC_DS3231 rtc;


tmElements_t te;  //Time elements structure
time_t unixTime; // a time stamp
time_t DosageTime;
time_t Reminder1;
time_t Reminder2;
time_t Reminder3;
time_t MissedDosage;
time_t LCDreminder;


UnixTime stamp(3); 


String DateHolder;
String DayToday;
String MonthToday;
String YearToday;
String HourToday;
String MinToday;
String SecToday;

bool isCounter1Finished=false;
bool isCounter2Finished=false;
bool isCounter3Finished=false;
bool isCounter4Finished=false;
bool isCounter5Finished=false;
bool isCounter6Finished=false;

bool DoseLogHasBeenSaved=false;
 
char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
 
int Day; 
int Month;
int Year; 
int Secs;
int Minutes;
int Hours;
 
String dofweek; // day of week
 
String myDate; 
String myTime; 
 
LiquidCrystal_I2C lcd(0x27, 20, 4);
Servo servo;

const int angle = 0;
const int servomotor = 8;
const int IRSensor = 2; // connect ir sensor to arduino pin 2
const int LED = 13; // conect Led to arduino pin 13
const int buzzer = 9; //buzzer to arduino pin 9
const int btn = 5; //Push button to arduino pin 5

// Push button Flags
bool isSetupTime = false;
bool isSetupYear = false;
bool isSetupMonth = false;
bool isSetupDay = false;
bool isSetupHour = false;
bool isSetupMin = false;
bool isSetupSec = false;

bool isBuzzerHigh = false;

bool isSetupDosetime =false;
bool isSetupDoseYear = false;
bool isSetupDoseMonth = false;
bool isSetupDoseDay = false;
bool isSetupDoseHour = false;
bool isSetupDoseMin = false;
bool isSetupDoseSec = false;

bool SetupDosetimeCompleted = false;
bool SetupTimeCompleted = false;

bool medicineHasbeenGiven = false;

String dosetime;
int setYear;
int setMonth;
int setDay;
int setHour;
int setMin;
int setSec;


File myFile;
File DataLog;
File VialsLog;
File MedicineSchedLog;
File PatientLog;

bool TimeNeededToBeSetup;
 
int _timeout;
String OutputString;
String _buffer;
String number = "+639264562589"; //-> change with your number

String DayNotif; 
String MonthNotif;
String YearNotif; 
String SecsNotif;
String MinutesNotif;
String HoursNotif;
int DayofWeekValue;
String DaTeNotification;
String TimeNotification;
String DayofWeekNotif;

String doseTime;
int vialsData;
String dataLine;

int btnVal;
int analogValue;
int analogYear;
int analogMonth; 
int analogDay;
int analogHour;
int analogMin;
int analogSec; 
int statusSensor;
// function to convert potentiometer value
int floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//rtc.adjust(DateTime(2022, 4, 15, 23, 39, 0));
void setup(){
  //Initialize SD card
  Serial.begin(4800);
  delay(7000); //delay for 7 seconds to make sure the modules get the signal
  _buffer.reserve(200);
   Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  Serial3.begin(4800);
  delay(1000);
  // get data

  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  pinMode(btn, INPUT_PULLUP); // set btn as input_pullup - pin 5
  pinMode (IRSensor, INPUT); // sensor pin INPUT
  pinMode (LED, OUTPUT); // Led pin OUTPUT
  //servo
  servo.attach(servomotor);
  servo.write(angle);

  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the LCD backlight
  delay(3000); // wait for console opening

  // RTC Module
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
 
  
  lcd.clear();
  lcd.setCursor(4,0);
  lcd.print("MEDBOX");

  RecieveMessage();
  SendMessage("This is Medbox reminding you to take your medicine on time!");
  getData();

    if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop(){
  
  statusSensor = digitalRead(IRSensor);
  DateTime now = rtc.now();
  Day = now.day(); 
  Month = now.month(); 
  Year = now.year();
  Secs = now.second(); 
  Hours = now.hour(); 
  Minutes = now.minute(); 
  dofweek = daysOfTheWeek[now.dayOfTheWeek()];  
  
  myDate = myDate + Month + "/" + Day + "/" + Year ; 
  myTime = myTime + Hours +":"+ Minutes +":" + Secs ; 
  SaveSMSLogs();

  // call getData() function if new data has been saved in the sd card
  if (DoseLogHasBeenSaved){
    getData();
    DoseLogHasBeenSaved= false;
  }

  Serial.println(DosageTime);
  Serial.println(now.unixtime());

  if (DosageTime < now.unixtime() && !medicineHasbeenGiven && !TimeNeededToBeSetup){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pls setup time");
    lcd.setCursor(0,1);
    lcd.print("of next dosage");
    SendMessage("Please setup time of next dosage ");
    TimeNeededToBeSetup =true;
    
  }
  if (LCDreminder == now.unixtime()){
     SendMessage("Notification: Medicine will be given after 5 minutes! Please take medicine on time! ");
     String Logs = "Notification: Medicine will be given after 5 minutes! Please take medicine on time! " + myDate+" "+ myTime;
     delay(3000);
     PatientDosageLog(Logs);
     delay(3000);
     SaveAllLogs(Logs);
     delay(3000);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("Medicine will be");
     lcd.setCursor(0,1);
     lcd.print("given in 5 mins");
     delay(10000);
     DisplayDosageTime();
  }
  if (DosageTime == now.unixtime()){
    SendMessage("Its time to take medicine ");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Its time to");
    lcd.setCursor(0,1);
    lcd.print("take medicine");
    servo.write(180);
    digitalWrite(LED, HIGH); // LED High
    tone(buzzer,1000);
    isBuzzerHigh = true;
    medicineHasbeenGiven = true;
    int NoOfVials = vialsData -1;
    String UpdateVialsData = String(NoOfVials) + " Vials is loaded";
    String Logs = "Medicine has been given on " + myDate+" "+ myTime;
    delay(3000);
    PatientDosageLog(Logs);
    delay(3000);
    UpdatingVialsData(UpdateVialsData);
    delay(3000);
    SaveAllLogs(Logs);
    servo.write(angle);
  }
  if (Reminder1 == now.unixtime() && medicineHasbeenGiven){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please take");
    lcd.setCursor(0,1);
    lcd.print("medicine on time");
    SendMessage("Patient havent taken the medicine ");
    String Logs = "Patient havent taken the medicine " + myDate+" "+ myTime;
    delay(3000);
    PatientDosageLog(Logs);
    delay(3000);
    SaveAllLogs(Logs);
  }
  if (Reminder2 == now.unixtime() && medicineHasbeenGiven){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please take");
    lcd.setCursor(0,1);
    lcd.print("medicine on time!");
    SendMessage("Patient still havent taken the medicine ");
    String Logs = "Patient still havent taken the medicine " + myDate+" "+ myTime;
    PatientDosageLog(Logs);
    SaveAllLogs(Logs);
  }
  if (Reminder3 == now.unixtime() && medicineHasbeenGiven){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Please take");
    lcd.setCursor(0,1);
    lcd.print("medicine on time!");
    SendMessage("Dont miss your dose take your medicine now ");
    String Logs = "Dont miss your dose take your medicine now " + myDate+" "+ myTime;
    delay(3000);
    PatientDosageLog(Logs);
    delay(3000);
    SaveAllLogs(Logs);
  }
  if (MissedDosage == now.unixtime() && medicineHasbeenGiven) {
    MissedDose();
    digitalWrite(LED, LOW); // LED LOW
    noTone(buzzer);
    isBuzzerHigh = false;
    medicineHasbeenGiven= false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Pls setup time");
    lcd.setCursor(0,1);
    lcd.print("of next dosage");
    SendMessage("Please setup time of next dosage ");
    String Logs = "Please setup time of next dosage " + myDate+" "+ myTime;
    delay(3000);
    SaveAllLogs(Logs);
    TimeNeededToBeSetup=true;
  }

  // read value of push button
  btnVal = digitalRead(btn);
  // read value if potentiometer
  analogValue = analogRead(A5);
  // Setup Year, Momth, Day, hour, min, sec using potentiometer
  analogYear = floatMap(analogValue, 0, 1023, 2022, 2099);
  analogMonth = floatMap(analogValue, 0, 1023, 1, 12);
  analogDay = floatMap(analogValue, 0, 1023, 1, 31);
  analogHour = floatMap(analogValue, 0, 1023, 0, 23);
  analogMin = floatMap(analogValue, 0, 1023, 0, 59);
  analogSec = floatMap(analogValue, 0, 1023, 0, 59);

// if IR Sensor dont detect medecine led low and buzzer off and else led high buzzer on
  if (statusSensor == 0 && medicineHasbeenGiven ){
      SendMessage("Medicine is taken on ");
      String Logs = "Medicine is taken on " + myDate+" "+ myTime;
      PatientDosageLog(Logs);
      SaveAllLogs(Logs);
      digitalWrite(LED, LOW); // LED LOW
      noTone(buzzer);
      isBuzzerHigh = false;
      medicineHasbeenGiven= false;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pls setup time");
      lcd.setCursor(0,1);
      lcd.print("of next dosage!");
      TimeNeededToBeSetup=false;
  }
  // call PushButton() function
  PushButton();
  // Reset myDate and MyTime;
  myDate = "";   
  myTime = ""; 
}


void PushButton(){
  // Push button 
// initialize holder =1
  int holder = 1;
  while (btnVal==0){
      holder= holder + 1;
      delay(1000);
      
      btnVal = digitalRead(btn);
      if(btnVal ==1){
        break;
      }
  }
// Push button conditions
  if (holder>=4){
   isSetupTime = true;
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Setup Time");
   delay(5000);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print("Press button");
   lcd.setCursor(0,1);
   lcd.print("again to proceed");
   while(isSetupTime){
       int btnVal = digitalRead(btn);
        if(btnVal==0){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Set up Year");
          isSetupYear = true;
        }

        if (isSetupYear){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
              int btnVal = digitalRead(btn);
              int analogValue = analogRead(A5);
              int analogYear = floatMap(analogValue, 0, 1023, 2022, 2099);
              lcd.setCursor(4,1);
              lcd.print(analogYear);
              delay(500);
              if (btnVal ==0){
                setYear = analogYear;
                isSetupMonth= true;
                break;
              }
          }
        }

        if (isSetupMonth){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Set up Month");
            int btnVal = digitalRead(btn);
            int analogValue = analogRead(A5);
            int analogMonth = floatMap(analogValue, 0, 1023, 1, 13);
            lcd.setCursor(4,1);
            lcd.print(analogMonth);
            delay(500);
            if(btnVal==0){
              setMonth = analogMonth;
              isSetupDay = true;
                break;
            }
          }
        }

        if (isSetupDay){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Set up Day");
            int btnVal = digitalRead(btn);
            int analogValue = analogRead(A5);
            int analogDay = floatMap(analogValue, 0, 1023, 1, 32);
            lcd.setCursor(4,1);
            lcd.print(analogDay);
            delay(500);
            if(btnVal==0){
              setDay = analogDay;
              isSetupHour = true;
                break;
            }
          }
        }

        if (isSetupHour){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Set up Hour");
            int btnVal = digitalRead(btn);
            int analogValue = analogRead(A5);
            int analogHour = floatMap(analogValue, 0, 1023, 0, 24);
            lcd.setCursor(4,1);
            lcd.print(analogHour);
            delay(500);
            if(btnVal==0){
              setHour = analogHour;
              isSetupMin = true;
                break;
            }
          }
        }

        if (isSetupMin){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Set up Minutes");
            int btnVal = digitalRead(btn);
            int analogValue = analogRead(A5);
            int analogMin = floatMap(analogValue, 0, 1023, 0, 60);
            lcd.setCursor(4,1);
            lcd.print(analogMin);
            delay(500);
            if(btnVal==0){
              setMin= analogMin;
              isSetupSec = true;
                break;
            }
          }
        }

        if (isSetupSec){
          int btnVal = digitalRead(btn);
          while(btnVal==1){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Set up Seconds");
            int btnVal = digitalRead(btn);
            int analogValue = analogRead(A5);
            int analogSec = floatMap(analogValue, 0, 1023, 0, 60);
            lcd.setCursor(4,1);
            lcd.print(analogSec);
            delay(500);
            if (btnVal==0){
              setSec = analogSec;
              SetupDosetimeCompleted =true;
                break;
            }
          }
        }
        
        if (SetupDosetimeCompleted){
          String mo = "Set medicine " + String(setMonth);
          String d = "/" + String(setDay);
          String y = "/" + String(setYear);
          String h = " " + String(setHour);
          String m = ":"+String(setMin);
          String s = ":"+String(setSec);
          dosetime = mo + d + y + h  + m  + s;
          if(SD.exists("DOSE_LOG.txt")){
            SD.remove("DOSE_LOG.txt");
          }
          MedicineSchedLog = SD.open("DOSE_LOG.txt", FILE_WRITE);
          if (MedicineSchedLog) {
            MedicineSchedLog.print(dosetime);
            MedicineSchedLog.close();
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("DOSE TIME ");
            lcd.setCursor(0,1);
            lcd.print("HAS BEEN SAVED");
            dosetime = "";
            setMonth =0;
            setDay = 0;
            setYear=0;
            setHour =0;
            setMin=0;
            setSec =0;
            isSetupTime = false;
          } else {
            lcd.print("error opening File");
            isSetupTime = false;
          }
          lcd.clear();
          lcd.print("Dosage Time");
          lcd.setCursor(0,1);
          lcd.print("has been setup");
          delay(5000);
          lcd.clear();
          DoseLogHasBeenSaved = true;
        }
   }

  } else if ((holder==2 || holder==3) && !isSetupTime && isBuzzerHigh){
    noTone(buzzer);
    isBuzzerHigh = false;
    digitalWrite(LED, LOW); // LED LOW
  }

}


//Send Message funtion that will send SMS message when called
void SendMessage(String in)
{
  //Serial.println ("Sending Message");
  Serial3.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  Serial3.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = in +myDate+" "+myTime;
  Serial3.println(SMS);
  delay(100);
  Serial3.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
}

//This function will 
void MissedDose()
{
  //Serial.println ("Sending Message");
  Serial3.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  //Serial.println ("Set SMS Number");
  Serial3.println("AT+CMGS=\"" + number + "\"\r"); //Mobile phone number to send message
  delay(1000);
  String SMS = "Patient missed the dose of "+myDate+" "+myTime+" medicine";
  Serial3.println(SMS);
  delay(100);
  Serial3.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
  _buffer = _readSerial();
  delay(3000);
  PatientDosageLog(SMS);
  delay(3000);
  SaveAllLogs(SMS);
}

void RecieveMessage()
{
  Serial.println ("Serial3800L Read an SMS");
  delay (1000);
  Serial3.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  Serial.write ("Unread Message done");
}
String _readSerial() {
  if (Serial3.available()>0) {
    return Serial3.readString();
  }
}

void SaveSMSLogs(){
    if (Serial3.available() > 0){
    OutputString =Serial3.readString(); 
    Serial.println(OutputString);
    if(OutputString.indexOf("+CMT:") > 0){
      myFile = SD.open("ALL_LOG.txt", FILE_WRITE);
      // if the file opened okay, write to it:
      if (myFile) {
        myFile.println(OutputString);
        // close the file:
        myFile.close();
      } else {
        Serial.println("error opening ALL_LOG.txt");
      }
        Serial.println(OutputString.indexOf("Vials"));
      if(OutputString.indexOf("Data ") > 0){
        DataLog = SD.open("DATA_LOG.txt", FILE_WRITE);
        DataLog.println(OutputString);
        DataLog.close();
      }else if(OutputString.indexOf("Vials") > 0){
        if(SD.exists("VIAL_LOG.txt")){
          SD.remove("VIAL_LOG.txt");
        }
        VialsLog = SD.open("VIAL_LOG.txt", FILE_WRITE);
        VialsLog.println(OutputString);
        VialsLog.close();
      }else if(OutputString.indexOf("Set medicine") > 0){
        if(SD.exists("DOSE_LOG.txt")){
          SD.remove("DOSE_LOG.txt");
        }
        MedicineSchedLog = SD.open("DOSE_LOG.txt", FILE_WRITE);
        MedicineSchedLog.println(OutputString);
        MedicineSchedLog.close();
        DoseLogHasBeenSaved=true;
      }
      
    }
  }
}

void getData(){
    // re-open the file for reading:
  VialsLog = SD.open("VIAL_LOG.txt");
  if (VialsLog) {
    // read from the file until there's nothing else in it:
    int counter = 0;
    while (VialsLog.available()) {
      dataLine = VialsLog.readStringUntil('\n');
      if (dataLine.indexOf("Vials")>0){
        int spacebar = dataLine.indexOf(" ");
        vialsData = dataLine.substring(0,spacebar).toInt();
        Serial.println(vialsData);
      }
    }
    // close the file:
    VialsLog.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening VIAL_LOG.txt");
  }
  MedicineSchedLog = SD.open("DOSE_LOG.txt");
  if (MedicineSchedLog) {
      // read from the file until there's nothing else in it:
      DayToday="";
      MonthToday="";
      YearToday="";
      HourToday="";
      MinToday="";
      SecToday="";
      DateHolder="";
      isCounter1Finished=false;
      isCounter2Finished=false;
      isCounter3Finished=false;
      isCounter4Finished=false;
      isCounter5Finished=false;
      isCounter6Finished=false;
      int counter = 0;
      while (MedicineSchedLog.available()) {
        dataLine = MedicineSchedLog.readStringUntil('\n');
        if (dataLine.indexOf("medicine") > 0){
          int len = dataLine.length();
          doseTime = dataLine.substring(13,len);
          delay(3000);
          char dostTimeHolder[doseTime.length()];
          for(int i=0; i<=doseTime.length()-1;i++){
            dostTimeHolder[i]=doseTime[i];
          }
          int counter =0;
          for(int i=0;i<=sizeof(dostTimeHolder)-1;i++){
            if(isDigit(dostTimeHolder[i])){
               if(counter ==1 && !isCounter1Finished){
                MonthToday=DateHolder;
                DateHolder="";
                isCounter1Finished = true;
               }else if(counter ==2 && !isCounter2Finished){
                DayToday=DateHolder;
                DateHolder="";
                isCounter2Finished = true;
                }else if(counter ==3 && !isCounter3Finished){
                 YearToday=DateHolder; 
                 DateHolder="";
                 isCounter3Finished=true;
                }else if(counter ==4 && !isCounter4Finished){
                 HourToday=DateHolder; 
                 DateHolder="";
                 isCounter4Finished=true;
                }else if(counter ==5 && !isCounter5Finished){
                  MinToday=DateHolder;
                  DateHolder="";
                  isCounter5Finished=true;
                }
                DateHolder = DateHolder + dostTimeHolder[i];
            }else{
              counter++;
            }
            if((sizeof(dostTimeHolder)-1) == i ){
                Serial.println("LENGTH");
                Serial.println(DateHolder.length());
                Serial.println(DateHolder);
                SecToday=DateHolder;
                DateHolder="";
                isCounter6Finished =true;
            }
          }

          Serial.println(DayToday);
          Serial.println(MonthToday);
          Serial.println(YearToday);
          Serial.println(HourToday);
          Serial.println(MinToday);
          Serial.println(SecToday);

          te.Second = SecToday.toInt();
          te.Hour = HourToday.toInt(); //11 pm
          te.Minute = MinToday.toInt();
          te.Day = DayToday.toInt();
          te.Month = MonthToday.toInt();
          te.Year = YearToday.toInt() - 1970; //Y2K, in seconds = 946684800UL
          unixTime =  makeTime(te);
          if (unixTime< 1650000000){
          Serial.println("Error");
          delay(1000);
          DosageTime = unixTime;
          }
          else{
            DosageTime = unixTime - 39916800;
          }
          Serial.println(DosageTime);
          LCDreminder=DosageTime-30;
          Reminder1=DosageTime+30;
          Reminder2=DosageTime+60;
          Reminder3=DosageTime+90;
          MissedDosage=DosageTime+120;
          print_date_time();
          stamp.getDateTime(DosageTime);
          DisplayDosageTime();
        }
      }
      // close the file:
      MedicineSchedLog.close();
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening DOSE_LOG.txt");
    }
}

void print_date_time() { //easy way to print date and time
  char buf[40];
  sprintf(buf, "%02d/%02d/%4d %02d:%02d:%02d", day(), month(), year(), hour(), minute(), second());
  Serial.println(buf);
}

void PatientDosageLog(String in){
  PatientLog = SD.open("history.txt", FILE_WRITE);
  // if the file opened okay, write to it:
  if (PatientLog) {
    PatientLog.println(in);
    // close the file:
    PatientLog.close();
  } else {
    Serial.println("error opening history.txt");
  }
}

void UpdatingVialsData(String in){
  if(SD.exists("VIAL_LOG.txt")){
      SD.remove("VIAL_LOG.txt");
  }
  VialsLog = SD.open("VIAL_LOG.txt", FILE_WRITE);
  if(VialsLog) {
    VialsLog.println(in);
    VialsLog.close();
  }else{
     Serial.println("error opening VIAL_LOG.txt");
  }
}

void SaveAllLogs(String in){
  myFile = SD.open("ALL_LOG.txt", FILE_WRITE);
  if (myFile) {
    myFile.println(in);
    myFile.close();
  } else {
    Serial.println("error opening ALL_LOG.txt");
  }
}

void DisplayDosageTime(){
  DaTeNotification="";
  TimeNotification="";
  lcd.clear();
  DayofWeekValue=stamp.dayOfWeek;
  switch (DayofWeekValue) {
  case 1:
    DayofWeekNotif = "Mon";
    break;
  case 2:
    DayofWeekNotif = "Tue";
    break;
  case 3:
    DayofWeekNotif = "Wed";
    break;
  case 4:
    DayofWeekNotif = "Thu";
    break;
   case 5:
    DayofWeekNotif = "Fri";
    break;
   case 6:
    DayofWeekNotif = "Sat";
    break;
   case 7:
    DayofWeekNotif = "Sun";
    break;
  default:
    DayofWeekNotif = "";
    break;
  }
  DaTeNotification= DayofWeekNotif +" " + MonthToday +"/" +  DayToday +"/"+YearToday;
  TimeNotification=  HourToday+":"+MinToday+":"+SecToday;
  lcd.setCursor(0,0);
  lcd.print(DaTeNotification);
  lcd.setCursor(0,1);
  lcd.print(TimeNotification);
}
