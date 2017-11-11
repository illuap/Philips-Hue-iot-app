#include <stdio.h>
#include <iostream>
#include <vector>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/Http/Message>
#include <Wt/Http/Client>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/WLogger>
#include <algorithm>

#include "BridgeControl.h"
#include "Session.h"

using namespace Wt;
using namespace std;

string ip = "ip";
string port = "port";
string name = "";
string location = "";

BridgeControlWidget::BridgeControlWidget(Session *session, WContainerWidget *parent):
  WContainerWidget(parent),
  session_(session)
{
  setContentAlignment(AlignCenter);
  setStyleClass("highscores");
}


void BridgeControlWidget::update()
{
	clear();
	
	this ->addWidget(new WBreak());

	//Database function to get the list of bridge ip addresses

	std::vector<Bridge> bridges = session_->getBridges();

	this->addWidget(new WText("Existing bridges:"));
	this->addWidget(new WBreak());
	for(int i = 0; i < bridges.size(); i++){
		this->addWidget(new WText(bridges[i].getBridgeName() + " " 
			+ bridges[i].getIpAddress() + ":" 
			+ std::to_string(bridges[i].getPortNumber())+"  "
			+ bridges[i].getUserId()));
		this->addWidget(new WBreak());
	}

	/*
	std::string userid = session_->getUserBridgeID();
	if(userid == ""){
		this->addWidget(new WText("NEW"));
	}else{
		this->addWidget(new WText("OLD"));
	}
	*/

	// END OF PRINTING BRIDGES
	
	//Input for the bridge name
	this->addWidget(new WText("Enter a bridge name: "));
	name_ = new WLineEdit(this);
	name_->setMargin(5, Left);
	this->addWidget(new WBreak());
	
	//Input for the ip address
	ip_ = new WText(this);
	ip_->setText("Enter the IP Address");
	ipEdit_ = new WLineEdit(this);
	ipEdit_->setMargin(5, Left);
	this->addWidget(new WBreak());

	//Input for the port number
	port_ = new WText(this);
	port_->setText("Enter the port number");
	portEdit_ = new WLineEdit(this);
	portEdit_->setMargin(5, Left);
	this->addWidget(new WBreak());

	//Input for the location
	location_ = new WText(this);
	location_->setText("Enter the location");
	locationEdit_ = new WLineEdit(this);
	locationEdit_->setMargin(5, Left);
	this->addWidget(new WBreak());

	//Register the bridge
	WPushButton *button
		= new WPushButton("Register", this);										
	button->setMargin(5, Left);

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	//Text displaying any errors
	errorText_ = new WText(this);

	this->addWidget(new WBreak());
	button->clicked().connect(this, &BridgeControlWidget::registerBridge);
	(boost::bind(&BridgeControlWidget::registerBridge, this));
	(boost::bind(&BridgeControlWidget::handleHttpResponse, this));
	(boost::bind(&BridgeControlWidget::connect, this));

}

//creates a client
Http::Client * BridgeControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handles post register requests and sets the bridge's registered status and user id
void BridgeControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//Parse the username from the response JSON
		size_t pos = response.body().find("username");
		string substring = response.body().substr(pos+11);
		size_t end = substring.find("\"");
		username = substring.substr(0,end);
		
		
		//Get the bridge name, ip address, port number, and location
		name = name_-> text().toUTF8();
		ip = ipEdit_->text().toUTF8();
		port = portEdit_->text().toUTF8();
		location = locationEdit_->text().toUTF8();

		//Creates a new bridge and adds it to the database
		Bridge *temp = new Bridge();
		temp->setBridgeName(name);
		temp->setIpAddress(ip);
		int x = stoi(port);
		temp->setPortNumber(x);
		temp->setUserId(username);
		temp->setLocation(location);

		session_->addBridge(temp);
		update();

	}
}

void BridgeControlWidget::registerBridge() {
	string name = name_->text().toUTF8();
	string ip = ipEdit_->text().toUTF8();
	string port = portEdit_->text().toUTF8();

	//Checks if entered port number is an integer
	if (!port.empty() && find_if(port.begin(), port.end(), [](char c) { return !std::isdigit(c); }) == port.end()) {
		vector<Bridge> bridges = session_->getBridges();
		//Makes a POST request to register the bridge if there are no bridges registered yet
		if (bridges.empty()) {
			Http::Client *client = BridgeControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"devicetype\" : \"danny\"}");
			client->done().connect(boost::bind(&BridgeControlWidget::handleHttpResponse, this, _1, _2));
			client->post("http://" + ip + ":" + port + "/api", *msg);
			testText_->setText(username + ip + port);
		}

		else {
			vector<int> ports;
			bool foundPort = false;
			//Creates a vector of all the port numbers
			for (size_t i = 0; i<bridges.size(); i++) {
				ports.push_back(bridges[i].getPortNumber());
			}

			//Makes a POST request to register the bridge if the bridge with the given port number isn't registered yet
			for (size_t i = 0; i<ports.size(); i++) {
				if (ports[i] == stoi(port)) {
					foundPort = true;
				}
			}

			if (foundPort == false) {
				Http::Client *client = BridgeControlWidget::connect();
				Http::Message *msg = new Http::Message();
				msg->addBodyText("{\"devicetype\" : \"danny\"}");
				client->done().connect(boost::bind(&BridgeControlWidget::handleHttpResponse, this, _1, _2));
				client->post("http://" + ip + ":" + port + "/api", *msg);
				testText_->setText(username + ip + port);
			}

			//Displays an error if the bridge is already registered
			else {
				errorText_->setText("Bridge already registered");
			}
		}
	}

	//Displays an error if an integer was not entered for the port number
	else {
		errorText_->setText("Not a valid port number");
	}
}

void BridgeControlWidget::showLights() 
{
	clear();
	WApplication::instance()->setInternalPath("/lights?user="+username+"&ip="+ip+"&port="+port,  true);
	
}