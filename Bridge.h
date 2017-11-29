#pragma once
#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>
#include <vector>

#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>

#include "User.h"

class User;

class Bridge {
private:
	std::string bridgeName;
	std::string location;
	std::string ipAddress;
	std::string hostName;
	std::string userId;
	bool registered;
	int portNumber;

public:
	Bridge() {
		bridgeName = "";
		location= "";
		ipAddress= "";
		hostName= "";
		userId= "";
		registered= false;
		portNumber= 0;
	}
	~Bridge() {}

	std::string getBridgeName() {
		return bridgeName;
	}

	std::string getLocation() {
		return location;
	}

	std::string getIpAddress() {
		return ipAddress;
	}

	std::string getHostName() {
		return hostName;
	}
	std::string getUserId() {
		return userId;
	}

	bool getRegistered() {
		return registered;
	}

	int getPortNumber() {
		return portNumber;
	}

	void setBridgeName(std::string newName) {
		bridgeName = newName;
	}

	void setLocation(std::string newLocation) {
		location = newLocation;
	}

	void setIpAddress(std::string newIpAddress) {
		ipAddress = newIpAddress;
	}

	void setHostName(std::string newHostName) {
		hostName = newHostName;
	}

	void setUserId(std::string newUserId) {
		userId = newUserId;
	}

	void setRegistered(bool newRegistered) {
		registered = newRegistered;
	}
	void setPortNumber(int newPortNumber) {
		portNumber = newPortNumber;
	}

	template<class Action>
	void persist(Action& a)
	{
		Wt::Dbo::field(a, bridgeName, "bridgeName");
		Wt::Dbo::field(a, location, "location");
		Wt::Dbo::field(a, ipAddress, "ipAddress");
		Wt::Dbo::field(a, hostName, "hostName");
		Wt::Dbo::field(a, userId, "userId");
		Wt::Dbo::field(a, registered, "registered");
		Wt::Dbo::field(a, portNumber, "portNumber");
	}
};
#endif	/* BRIDGE_H */