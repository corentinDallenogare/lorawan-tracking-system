/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2018 AllThingsTalk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */ 

/*
 * Do you want to get notified when someone is moving things precious to
 * you? This experiment shows how a LoRa device can be used to detect
 * unexpected movement of an object and send out notifications to its
 * owner. Furthermore, it shows how you can track the object using its
 * GEO location.
 */
 
// Select your preferred method of sending data
#define CONTAINERS
//#define CBOR
//#define BINARY

/***************************************************************************/



#include <Wire.h>
#include <MMA7660.h>
#include <ATT_LoRaWAN.h>
#include <ATT_GPS.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define Serial_BAUD 57600

#define debugSerial Serial
#define loraSerial Serial1

#define ACCEL_THRESHOLD 12  // Threshold for accelerometer movement
#define DISTANCE 30.0       // Minimal distance between two fixes to keep checking gps
#define FIX_DELAY 6000000     // Delay (ms) between checking gps coordinates

MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);  // minimum time between 2 messages set at 7000 milliseconds

#ifdef CONTAINERS
  #include <Container.h>
  Container container(device);
#endif
/*
#ifdef CBOR
  #include <CborBuilder.h>
  CborBuilder payload(device);
#endif

#ifdef BINARY
  #include <PayloadBuilder.h>
  PayloadBuilder payload(device);
#endif*/


int digitalSensor = 22;
ATT_GPS gps(20,21);  // Reading GPS values from debugSerial connection with GPS

MMA7660 accelerometer;
bool moving = false;  // Set given data from both accelerometer and gps

// variables for the coordinates (GPS)
float prevLatitude;
float prevLongitude;

int8_t prevX,prevY,prevZ;  // Keeps track of the previous accelerometer data   

void setup() 
{
  // accelerometer is always running so we can check when the object is moving
  accelerometer.init();

  debugSerial.begin(Serial_BAUD);
  while((!debugSerial) && (millis()) < 10000){}  // wait until the serial bus is available
  
  loraSerial.begin(modem.getDefaultBaudRate());  // set baud rate of the serial connection to match the modem
  while((!loraSerial) && (millis()) < 10000){}   // wait until the serial bus is available

  while(!device.initABP(DEV_ADDR, APPSKEY, NWKSKEY))
  debugSerial.println("Ready to send data");

  debugSerial.println();
  debugSerial.println("-- Charging --");
  debugSerial.println();
    
  debugSerial.print("Initializing GPS");
  readCoordinates();
  debugSerial.println("Done");

  accelerometer.getXYZ(&prevX, &prevY, &prevZ);  // get initial accelerometer state

  debugSerial.println("Ready to track animals");
  debugSerial.println();
}

void process()
{
  while(device.processQueue() > 0)
  {
    debugSerial.print("QueueCount: ");
    debugSerial.println(device.queueCount());
    delay(10000);
  }
}

unsigned long sendNextAt = 0;  // Keep track of time
void loop()
{
  if(!moving)  // If not moving, check accelerometer
  {
    moving = isAccelerating();
    delay(500);
  }

  if(moving && sendNextAt < millis())  // We waited long enough to check new fix
  {
    readCoordinates();
    
    if(gps.calcDistance(prevLatitude, prevLongitude) <= DISTANCE)  // We did not move much. Back to checking accelerometer for movement
    {
      debugSerial.print("Less than ");
      debugSerial.print(DISTANCE);
      debugSerial.println(" movement in last 5 minutes");
      moving = false;
      sendCoordinates(false);  // Send fix and motion false
    }
    else  // Update and send new coordinates
    {
      prevLatitude = gps.latitude;
      prevLongitude = gps.longitude;
      sendCoordinates(true);  // Send fix and motion true
    }
    sendNextAt = millis() + FIX_DELAY;  // Update time
  }
}

// Check if acceleration is detected
bool isAccelerating()
{
  int8_t x,y,z;
  accelerometer.getXYZ(&x, &y, &z);
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > ACCEL_THRESHOLD;
  
  if(result == true)
  {
    prevX = x;
    prevY = y;
    prevZ = z;
  }

  return result; 
}

// Try reading GPS coordinates
void readCoordinates()
{
  while(gps.readCoordinates() == false)
  {
    debugSerial.print(".");
    delay(1000);                 
  }
  debugSerial.println();
}

// Send the GPS coordinates to the AllThingsTalk cloud
void sendCoordinates(boolean val)
{
  
  #ifdef CONTAINERS
  container.addToQueue(val, BINARY_SENSOR, false); process();
  container.addToQueue(gps.latitude, gps.longitude, gps.altitude, gps.timestamp, GPS, false); process();
  #endif
  /*
  #ifdef CBOR  // Send data using Cbor
  payload.reset();
  payload.map(2);
  payload.addBoolean(val, "1");
  payload.addGPS(gps.latitude, gps.longitude, gps.altitude, "9");
  payload.addToQueue(false);
  process();
  #endif
  
  #ifdef BINARY  // Send data using a Binary payload and our ABCL language
  payload.reset();
  payload.addBoolean(val);
  payload.addGPS(gps.latitude, gps.longitude, gps.altitude);
  payload.addToQueue(false);
  process();
  #endif*/
  
  debugSerial.print("lng: ");
  debugSerial.print(gps.longitude, 4);
  debugSerial.print(", lat: ");
  debugSerial.print(gps.latitude, 4);
  debugSerial.print(", alt: ");
  debugSerial.print(gps.altitude);
  debugSerial.print(", time: ");
  debugSerial.println(gps.timestamp);
}
