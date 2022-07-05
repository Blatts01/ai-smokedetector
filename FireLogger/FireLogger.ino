#include <sps30.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include "Adafruit_SGP30.h"
#include "Adafruit_SHT31.h"
#include "Arduino_BHY2.h"

/******************************/
/*Defines*/
#define SEALEVELPRESSURE_HPA (1013.25)

/******************************/
/*Global Variables*/
Adafruit_BMP3XX bmp388;
Adafruit_SGP30 sgp30;
Adafruit_SHT31 sht31;

bool enableHeater = false;
const int chipSelect = 4;
uint32_t loopCnt = 0;
struct sps30_measurement m;
char serial[SPS30_MAX_SERIAL_LEN];
uint16_t data_ready;
int16_t ret;
uint8_t auto_clean_days = 4;
uint32_t auto_clean;
String dataString = "";
File dataFile;


const int buttonPinFire = 10;     
const int buttonPinPizza = 11;    
int buttonStateFire = 0;         
int buttonStatePizza = 0; 


/******************************/
/*Helper Functions*/

/* return absolute humidity [mg/m^3] with approximation formula
* @param temperature [°C]
* @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

/******************************/
/*Setup Function*/

void setup() {
  Serial.begin(9600);

  pinMode(buttonPinPizza, INPUT);
  pinMode(buttonPinFire, INPUT);
  
  delay(2000);

  /*Sensirion SPS30 Setup*/
  sensirion_i2c_init();
  while (sps30_probe() != 0) {
    Serial.print("SPS sensor probing failed\n");
    delay(500);
  }
  
  ret = sps30_set_fan_auto_cleaning_interval_days(auto_clean_days);
  if (ret) {
    Serial.print("error setting the auto-clean interval: ");
    Serial.println(ret);
  }

  ret = sps30_start_measurement();
  if (ret < 0) {
    Serial.print("error starting measurement\n");
    // don't do anything more:
    while (1);
  }
  Serial.print("Found SPS30 with Serial:");
  /**********************************************************************************/
  /*Sensirion BMP388 Setup*/
  if (!bmp388.begin_I2C()) {  
    Serial.println("Could not find a valid BMP388 sensor, check wiring!");
    while (1);
  }
  Serial.print("Found BMP388 with Serial:");
  // Set up oversampling and filter initialization
  bmp388.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp388.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp388.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp388.setOutputDataRate(BMP3_ODR_50_HZ);
  /**********************************************************************************/
  /*Sensirion SPG30 Setup*/
  if (! sgp30.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  Serial.print("Found SGP30 with Serial:");
  Serial.print(sgp30.serialnumber[0], HEX);
  Serial.print(sgp30.serialnumber[1], HEX);
  Serial.println(sgp30.serialnumber[2], HEX);

  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp30.setIAQBaseline(0x8E68, 0x8F41);  // Will vary for each sensor!
  /**********************************************************************************/
  /*Sensirion SHT31 Setup*/
  sht31 = Adafruit_SHT31();
  if (! sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }
  Serial.print("Found SHT31 with Serial:");
  Serial.print("SHT31 Heater State: ");
  if (sht31.isHeaterEnabled()){
    Serial.println("ENABLED");
  }
  else{
    Serial.println("DISABLED");
  }
  /**********************************************************************************/
  /*SD Card Setup*/

  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");

  char newfile[50];
  sprintf(newfile, "d_%04u.txt", millis());
  Serial.println(newfile);
  String csvHeader = "Temperature[C],Humidity[%],TVOC[ppb],eCO2[ppm],Raw H2,Raw Ethanol,Pressure[hPa],PM1.0,PM2.5,NC0.5,NC1.0,NC2.5,Fire Alarm,Pizza Alarm,CNT,UTC";
  dataFile = SD.open(newfile, O_CREAT | O_EXCL | O_WRITE);
    // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(csvHeader);
    dataFile.close();
    // print to the serial port too:
    Serial.println(csvHeader);
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
    // don't do anything more:
    while (1);
  }
  delay(1000);
}

void loop() {

  dataString = "";
  float temperature = sht31.readTemperature();
  float humidity = sht31.readHumidity();

  sgp30.setHumidity(getAbsoluteHumidity(temperature, humidity));

  if (! sgp30.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }

  if (! sgp30.IAQmeasureRaw()) {
    Serial.println("Raw Measurement failed");
    return;
  }
  
  if (! bmp388.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }

  /*Read input buttons*/
  buttonStateFire = digitalRead(buttonPinFire);
  buttonStatePizza = digitalRead(buttonPinPizza);

  do {
    ret = sps30_read_data_ready(&data_ready);
    if (ret < 0) {
      Serial.print("error reading data-ready flag: ");
      Serial.println(ret);
    } else if (!data_ready)
      Serial.print("data not ready, no new measurement available\n");
    else
      break;
    delay(500); /* retry in 100ms */
  } while (1);

  ret = sps30_read_measurement(&m);
  if (ret < 0) {
    Serial.print("error reading measurement\n");
  } 
  else {

    dataString += String(temperature, 3) +",";
    dataString += String(humidity, 3) +",";
    dataString += String(sgp30.TVOC) +",";
    dataString += String(sgp30.eCO2) +",";
    dataString += String(sgp30.rawH2) +",";
    dataString += String(sgp30.rawEthanol) +",";
    dataString += String(bmp388.pressure / 100.0, 3) +",";
    dataString += String(m.mc_1p0) +",";
    dataString += String(m.mc_2p5) +",";
    dataString += String(m.nc_0p5) +",";
    dataString += String((m.nc_1p0  - m.nc_0p5), 3) +",";
    dataString += String((m.nc_2p5  - m.nc_1p0), 3) +",";
    if (buttonStateFire == HIGH) {
    // Fire
      dataString += "1,";
    } else {
    // No Fire
      dataString += "0,";
    }
    if (buttonStatePizza == HIGH) {
      // Pizza burning
      dataString += "1,";
    } else {
      // Pizza not burning
      dataString += "0,";
    }
    dataString += String(loopCnt) +",";
    dataString += String(loopCnt);
  }
  
  dataFile = SD.open("datalog.txt", FILE_WRITE);
    // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    if (loopCnt%1 == 0) {
      Serial.println(dataString);
    }
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }

  if ((loopCnt%30) == 0) {

    /*SHT31 Heater check*/
    enableHeater = !enableHeater;
    sht31.heater(enableHeater);
    Serial.print("SHT31 Heater Enabled State: ");
    if (sht31.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");

    /*SPG30 Baseline check*/
    uint16_t TVOC_base, eCO2_base;
    if (! sgp30.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
  }
  loopCnt++;
  delay(1000);

}
