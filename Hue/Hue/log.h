// log.h: Taken from the helper site on OWL and modified to implement a RESTful API with Wt
// Nicole Chow	nchow24		250725575
// Oct 3, 2017

#pragma once
#ifndef LOG_H_
#define LOG_H_

#include <Wt/WLogger>

void info(Wt::WLogger& logger, const std::string& message);
void fatal(Wt::WLogger& logger, const std::string& message, const char* what);
void fatal(Wt::WLogger& logger, const std::string& message);
void configLogger(Wt::WLogger& logger);

#endif /* LOG_H_ */
