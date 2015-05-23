// Copyright 2014-2015 the project authors as listed in the AUTHORS file.
// All rights reserved. Use of this source code is governed by the
// license that can be found in the LICENSE file.

#include "PI433.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "BluelineDevice.h"
#include "NexxTechDevice.h"
#include "LacrossTX141.h"
#include "Device2262.h"

#define NUM_REQUIRED_CERT_FILES 3 
     
int main(int argc, char *argv[]) {
   int pin = 2;
   char* certsDir = NULL;

   // validate we got the required parameters 
   if (2 > argc) {
      printf("Missing mqttBroker parameter - exiting\n");
      printf("Usage: PI433Main mqtt_url <cert info dir>\n");
      return -1;
   }

   if (strstr(argv[1], "ssl://") == argv[1]) {
      // ok we expect a third parameter which tells us where to look for the 
      // certificates and keys 
      // the names of the files in the directory specified must be client.cert client.key and ca.cert
      if (3 > argc) { 
         printf("Cert info dir required for connection to broker with ssl - exiting\n");
         printf("Usage: PI433Main mqtt_url <cert info dir>\n");
         return -1;
      }
 
      // validate the directory exists
      DIR* dirPtr = opendir(argv[2]);
      if(NULL == dirPtr) {
         printf("Cert info dir was invalid - exiting\n");
         return -2;
      } 
     
      struct dirent *nextEntry;
      int requiredFilesFound = 0;
      while ((nextEntry = readdir(dirPtr)) != NULL) {
         if ((strcmp(nextEntry->d_name, CA_CERT_FILE) == 0) ||
             (strcmp(nextEntry->d_name, CLIENT_CERT_FILE) == 0) ||
             (strcmp(nextEntry->d_name, CLIENT_KEY_FILE) == 0)) {
            requiredFilesFound++;
         }
      }
      closedir(dirPtr);

      if (requiredFilesFound != NUM_REQUIRED_CERT_FILES) { 
         printf("Missing cert info files, directory must contain client.key, client.cert and ca.cert\n");
         return -3;
      }

      certsDir = argv[2];
   }

   if(wiringPiSetup() == -1) {
      return -4;
   }

   PI433 receiver = PI433(pin, argv[1], certsDir);

   // setup devices, TODO make configurable
   receiver.registerDevice(new BluelineDevice(0x1efd)); //use houseCode when paper in way, reversed
   receiver.registerDevice(new NexxTechDevice());
   receiver.registerDevice(new LacrossTX141());
   receiver.registerDevice(new Device2262());
   receiver.handleMessages();
}

