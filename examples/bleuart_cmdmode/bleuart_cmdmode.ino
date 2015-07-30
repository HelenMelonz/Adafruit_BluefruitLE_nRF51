/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Wire.h>
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_LSM9DS0.h>
#include <Adafruit_Sensor.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

// Create the bluefruit object, either software serial...uncomment these lines
/*
SoftwareSerial bluefruitSS = SoftwareSerial(BLUEFRUIT_SWUART_TXD_PIN, BLUEFRUIT_SWUART_RXD_PIN);

Adafruit_BluefruitLE_UART ble(bluefruitSS, BLUEFRUIT_UART_MODE_PIN,
                      BLUEFRUIT_UART_CTS_PIN, BLUEFRUIT_UART_RTS_PIN);
*/

/* ...or hardware serial, which does not need the RTS/CTS pins. Uncomment this line */
 Adafruit_BluefruitLE_UART ble(Serial1, BLUEFRUIT_UART_MODE_PIN);

/* ...hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/* ...software SPI, using SCK/MOSI/MISO user-defined SPI pins and then user selected CS/IRQ/RST */
//Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_SCK, BLUEFRUIT_SPI_MISO,
//                             BLUEFRUIT_SPI_MOSI, BLUEFRUIT_SPI_CS,
//                             BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, 6, NEO_GRB + NEO_KHZ800);
// i2c
Adafruit_LSM9DS0 lsm = Adafruit_LSM9DS0();

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_2000DPS);
}

/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit Command Mode Example"));
  Serial.println(F("---------------------------------------"));

  strip.begin();
  strip.show();
  strip.setPixelColor(0, strip.Color(0, 0, 7)); // 
  strip.show();

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("*****************"));
  
  Serial.println("LSM raw read demo");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    Serial.println("Oops ... unable to initialize the LSM9DS0. Check your wiring!");
    while (1);
  }
  Serial.println("Found LSM9DS0 9DOF");
  Serial.println("");
  Serial.println("");
  
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
 //TX Talk to bluetooth

  ble.print("AT+BLEUARTTX=");
  lsm.read();
  ble.println(
    "Accel X: " + (String)(int)lsm.accelData.x + 
    " Y: " + (String)(int)lsm.accelData.y + 
    " Z: " + (String)(int)lsm.accelData.z
    );
    ble.println();
//  ble.print("Mag X: "); ble.print((int)lsm.magData.x);     ble.print(" ");
//  ble.print("Y: "); ble.print((int)lsm.magData.y);         ble.print(" ");
//  ble.print("Z: "); ble.println((int)lsm.magData.z);       ble.print(" ");
//  ble.print("Gyro X: "); ble.print((int)lsm.gyroData.x);   ble.print(" ");
//  ble.print("Y: "); ble.print((int)lsm.gyroData.y);        ble.print(" ");
//  ble.print("Z: "); ble.println((int)lsm.gyroData.z);      ble.println(" ");
//  ble.print("Temp: "); ble.print((int)lsm.temperature);    ble.println(" ");
  ble.waitForOK();
  
//  // Check for user input
//  char inputs[BUFSIZE+1];
//
//  if ( getUserInput(inputs, BUFSIZE) )
//  {
//    // Send characters to Bluefruit
//    Serial.print("[Send] ");
//    Serial.println(inputs);
//
//    ble.print("AT+BLEUARTTX=");
//    ble.println(inputs);
//
//    // check response stastus
//    if (! ble.waitForOK() ) {
//      Serial.println(F("Failed to send?"));
//    }
//  }
//
//  // Check for incoming characters from Bluefruit
//  ble.println("AT+BLEUARTRX");
//  ble.readline();
//  if (strcmp(ble.buffer, "OK") == 0) {
//    // no data
//    return;
//  }
//  // Some data was found, its in the buffer
//  Serial.print(F("[Recv] ")); Serial.println(ble.buffer);
//  
//   byte r = ble.buffer[2];
//  Serial.print("r = ");
//  Serial.println(r, DEC);
//
//  byte g = ble.buffer[3];
//  Serial.print("g = ");
//  Serial.println(g, DEC);
//
//  byte b = ble.buffer[4];
//  Serial.print("b = ");
//  Serial.println(b, DEC);
// 
//  strip.setPixelColor(0, strip.Color(r, g, b)); // 
//  strip.show();
//  
//  ble.waitForOK();
  delay(1000);

}

/**************************************************************************/
/*!
    @brief  Checks for user input (via the Serial Monitor)
*/
/**************************************************************************/
bool getUserInput(char buffer[], uint8_t maxSize)
{
  // timeout in 100 milliseconds
  TimeoutTimer timeout(100);

  memset(buffer, 0, maxSize);
  while( (Serial.peek() < 0) && !timeout.expired() ) {}

  if ( timeout.expired() ) return false;

  delay(2);
  uint8_t count=0;
  do
  {
    count += Serial.readBytes(buffer+count, maxSize);
    delay(2);
  } while( (count < maxSize) && !(Serial.peek() < 0) );

  return true;
}


