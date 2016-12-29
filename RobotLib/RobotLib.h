#pragma once 
#include <string>
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include "../3rdParty/humblelogging-master/include/humblelogging/api.h"
#include "../3rdParty/RapidXML/rapidxml_print.hpp"
#include "Database.h"
#include "DeviceManager.h"
class Database;
class DeviceManager;

// Defined if we are going to be running in test mode
// This will have the various managers get simulated data
#define SIMULATION

// Library class used throughout RobotLib
// Includes things like the logger
class RobotLib
{
	public:		
		RobotLib();
		void Log(std::string message);
		void LogWarn(std::string message);
		void LogError(std::string message);	
		void LogException(std::exception &e);
	
		DeviceManager* getDeviceManager()
		{
			return deviceManager;
		}
		Database * getDatabase()
		{
			return database;
		}
		bool getEmulator()
		{
			return emulator;
		}
		~RobotLib();
	
	private:
		void initLog();
		bool checkEmulator();
		Database *database;
		DeviceManager *deviceManager;		
		bool emulator;
};