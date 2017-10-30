#include <string>
#include <vector>
class Bridge {
private:
	std::string bridgeName;
	std::string location;
	std::string ipAddress;
	std::string hostName;
	int portNumber;
	std::vector<Group> groupList;
	std::vector<Light> lightList;

public:
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

	int getPortNumber() {
		return portNumber
	}

	void setBridgeName(std::string newName) {
		bridgeName = name;
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

	void setPortNumber(int newPortNumber) {
		portNumber = newPortNumber;
	}

	void addGroup(Group newGroup) {
		groupList.push_back(newGroup);
	}

	std::vector getGroups() {
		return groupList;
	}

	void removeGroup(Group group) {

	}
};
