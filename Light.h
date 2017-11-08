
#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <iostream>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>


class Light {
	//Light data members (name, type, status (on/off), brightness, hue, saturation, transition time)
	std::string name;
	std::string type;
	bool on;
	int bri;
	int hue;
	int sat;
	int transitionTime;

public:
	// Function Name: Light() 
	// Parameters: name, type, brightness, hue, saturation, transition time
	// Return: none
	// Description: constructor 
	Light(std::string nName, std::string nType, int nBri, int nHue, int nSat, bool nOn, int nTrans) {
		name = nName;
		type = nType;
		bri = nBri;
		hue = nHue;
		sat = nSat;
		on = nOn;
		transitionTime = nTrans;
	}

	// Function Name: Light() 
	// Parameters: none
	// Return: none
	// Description: empty constructor 
	Light() {
		name = "";
		type = "";
		bri = 0;
		hue = 0;
		sat = 0;
		on = 0;
		transitionTime = 0;
	}

	// Function Name: ~Light() 
	// Parameters: 
	// Return: none
	// Description: destructor
	~Light() {}

	// Function Name: getName()
	// Parameters: None
	// Return: string of the name
	// Description: returns the name
	std::string getName(void) {
		return name;
	}

	// Function Name: getType()
	// Parameters: None
	// Return: string of the type
	// Description: returns the type
	std::string getType(void) {
		return type;
	}

	// Function Name: getBrightness()
	// Parameters: None
	// Return: int of the brightness
	// Description: returns the brightness
	int getBrightness(void) {
		return bri;
	}

	// Function Name: getHue()
	// Parameters: None
	// Return: int of the hue
	// Description: returns the hue
	int getHue(void) {
		return hue;
	}

	// Function Name: getSaturation()
	// Parameters: None
	// Return: int of the saturation
	// Description: returns the saturation
	int getSaturation(void) {
		return sat;
	}

	// Function Name: getOn()
	// Parameters: None
	// Return: bool of on
	// Description: returns whether light is on/off
	bool getOn(void) {
		return on;
	}


	// Function Name: getTransition()
	// Parameters: None
	// Return: int of the transition time
	// Description: returns the transition time
	int getTransition(void) {
		return transitionTime;
	}

	void setName(std::string newName) {
		name = newName;
	}

	void setType(std::string newType) {
		type = newType;
	}

	void setBrightness(int newBri) {
		bri = newBri;
	}

	void setHue(int newHue) {
		hue = newHue;
	}

	void setSaturation(int newSat) {
		sat = newSat;
	}

	void setOn(bool newOn) {
		on = newOn;
	}

	void setTransition(int newTrans) {
		transitionTime = newTrans;
	}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, name, "name");
		Wt::Dbo::field(a, type, "type");
		Wt::Dbo::field(a, on, "on");
		Wt::Dbo::field(a, bri, "bri");
		Wt::Dbo::field(a, hue, "hue");
		Wt::Dbo::field(a, sat, "sat");
		Wt::Dbo::field(a, transitionTime, "transitionTime");
	}
};

#endif	/* LIGHT_H_ */