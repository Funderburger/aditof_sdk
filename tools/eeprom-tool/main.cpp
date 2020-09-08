#include <iostream>
#include <stdio.h>
#include <glog/logging.h>
#include <getopt.h>
#include <string>
#include <aditof/eeprom_factory.h>
#include <aditof/device_construction_data.h>
#include "eepromtoolcontroller.h"
#include <iterator>
#include <algorithm>

using namespace aditof;
using namespace std;

enum ActionType {
   WRITE,
   READ,
   UNKNOWN
};

typedef struct {
   string path;
   ConnectionType connectionType = ConnectionType::LOCAL;
   ActionType actionType = UNKNOWN;
} CLIArguments;

Status parseArguments(int argc, char *argv[], CLIArguments& cliArguments){
   int option;

   while((option = getopt(argc, argv, ":ue:mr:w:")) != -1){ //get option from the getopt() method
      switch(option){
         case 'u':
            cliArguments.connectionType = ConnectionType::USB;
            break;
         case 'e':
            cliArguments.connectionType = ConnectionType::ETHERNET;
            break;
            //TO DO: maybe rename this options to Local instead of mipi ?
         case 'm':
            cliArguments.connectionType = ConnectionType::LOCAL;
            break;
         case 'w': 
            cliArguments.actionType = WRITE;
            cliArguments.path = string(optarg);
            break;
         case 'r': 
            cliArguments.actionType = READ;
            cliArguments.path = string(optarg);
            break;
         case ':':
            printf("option needs a value\n");
            return Status::INVALID_ARGUMENT;
            break;
         case '?': //used for some unknown options
            printf("unknown option: %c\n", optopt);
            break;
      }
   }

   return Status::OK;
}

int main(int argc, char *argv[]){
   Status status;
   auto controller = std::make_shared<EepromToolController>();
   CLIArguments cliArguments;

   status = parseArguments(argc, argv, cliArguments);
   if (status != aditof::Status::OK){
      LOG(ERROR) << "cannot parse CLI arguments";
      return 1;
   }

   status = controller->setConnection(cliArguments.connectionType);
   if (status != aditof::Status::OK){
      LOG(ERROR) << "cannot set connection";
      return 1;
   }

   switch (cliArguments.actionType)
   {
   case READ:
         controller->readEepromToFile(cliArguments.path.c_str());
      break;
   case WRITE:
         controller->writeFileToEeprom(cliArguments.path.c_str());
      break;
   default:
      break;
   }

  return 0;
}
