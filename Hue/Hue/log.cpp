// log.cpp : Taken from the helper site on OWL and modified to implement a RESTful API with Wt
// Nicole Chow	nchow24		250725575
// Oct 3, 2017

#include <iostream>
#include "log.h"

using namespace std;
using namespace Wt;

void info(WLogger& logger, const string& message) {
	WLogEntry entry = logger.entry("info");
	entry << WLogger::timestamp << WLogger::sep << WLogger::sep << '[' << "info"
		<< ']' << WLogger::sep << message;
}
void fatal(WLogger& logger, const string& message, const char* what) {
	WLogEntry entry = logger.entry("fatal");
	entry << WLogger::timestamp << WLogger::sep << WLogger::sep << '['
		<< "fatal" << ']' << WLogger::sep << message << what;
}

void fatal(WLogger& logger, const string& message) {
	fatal(logger, message, "");
}

void configLogger(WLogger& logger) {
	logger.addField("datetime", false);
	logger.addField("type", false);
	logger.addField("message", true);
	logger.setFile("/var/log/resource.log");
}
