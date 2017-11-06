
#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <iostream>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>

class Light {
	//Light data members (name, type, status (on/off), brightness, hue, saturation, transition time)
	std::string type;
	std::string status;
	
	int hue;
	int sat;
	int transitionTime;

public:
	std::string name;
	int bri;
	// Function Name: Light() 
	// Parameters: name, type, status, brightness, hue, saturation, transition time
	// Return: none
	// Description: constructor 
	Light(std::string nName, std::string nType, std::string nStatus, int nBri, int nHue, int nSat, int nTrans);

	Light();
	// Function Name: ~Light() 
	// Parameters: 
	// Return: none
	// Description: destructor
	~Light() {}

	int getBri();


	  template<class Action>
	  void persist(Action& a)
	  {
	    Wt::Dbo::field(a, name, "name");
	    Wt::Dbo::field(a, type, "type");
	    Wt::Dbo::field(a, status, "status");
	    Wt::Dbo::field(a, bri, "bri");
	    Wt::Dbo::field(a, hue, "hue");
	    Wt::Dbo::field(a, transitionTime, "transitionTime");
	  }
};

#endif	/* LIGHT_H_ */