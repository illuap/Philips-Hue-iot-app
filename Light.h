
#pragma once
#ifndef LIGHT_H
#define LIGHT_H

#include <string>
#include <iostream>


class Light {
	//Light data members (name, type, status (on/off), brightness, hue, saturation, transition time)
	std::string name;
	std::string type;
	std::string status;
	int bri;
	int hue;
	int sat;
	int transitionTime;

public:
	// Function Name: Light() 
	// Parameters: name, type, status, brightness, hue, saturation, transition time
	// Return: none
	// Description: constructor 
	Light(std::string nName, std::string nType, std::string nStatus, int nBri, int nHue, int nSat, int nTrans) {
		name = nName;
		type = nType;
		status = nStatus;
		bri = nBri;
		hue = nHue;
		sat = nSat;
		transitionTime = nTrans;
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

	// Function Name: getStatus()
	// Parameters: None
	// Return: bool of the status
	// Description: returns the status
	std::string getStatus(void) {
		return status;
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
	int gethue(void) {
		return hue;
	}

	// Function Name: getSaturation()
	// Parameters: None
	// Return: int of the saturation
	// Description: returns the saturation
	int getSaturation(void) {
		return sat;
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

	void setStatus(bool newStatus) {
		status = newStatus;
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

	void setTransition(int newTrans) {
		transitionTime = newTrans;
	}
};

#endif	/* LIGHT_H_ */
