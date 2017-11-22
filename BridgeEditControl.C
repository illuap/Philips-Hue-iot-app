
#include <stdio.h>
#include <iostream>
#include <vector>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WString>
#include <Wt/Http/Message>
#include <Wt/Http/Client>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/WLogger>
#include <algorithm>

#include "BridgeEditControl.h"
#include "Session.h"
#include "Bridge.h"
#include "BridgeUserIds.h"

using namespace Wt;
using namespace std;

BridgeEditControlWidget::BridgeEditControlWidget(Session *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session)
{
	setContentAlignment(AlignCenter);
	setStyleClass("highscores");
}


void BridgeEditControlWidget::update()
{
	clear();
	//get URL info
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("ip=");								//get ip
	string subString = address.substr(pos + 3);
	size_t endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");								//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);

	thisBridge = new Bridge();
	thisBridge = session_->getBridge(ip,port);

	

	this->addWidget(new WText("Bridge Name: "));
	nameEdit_ = new WLineEdit(this);
	nameEdit_->setText(thisBridge->getBridgeName());
	this->addWidget(new WBreak());

	this->addWidget(new WText("IP Address: "));
	ipEdit_ = new WLineEdit(this);
	ipEdit_->setText(thisBridge->getIpAddress());
	this->addWidget(new WBreak());

	this->addWidget(new WText("Port Number: "));
	portEdit_ = new WLineEdit(this);
	portEdit_->setText(to_string(thisBridge->getPortNumber()));
	this->addWidget(new WBreak());

	this->addWidget(new WText("Location: "));
	locationEdit_ = new WLineEdit(this);
	locationEdit_->setText(thisBridge->getLocation());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	WPushButton *button
		= new WPushButton("Update Bridge", this);
	WPushButton *returnButton
		= new WPushButton("Return Bridge", this);
	returnButton->setLink("/?_=/bridge");
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	errorText_ = new WText(this);

	

	button->clicked().connect(this, &BridgeEditControlWidget::updateEdit);
	(boost::bind(&BridgeEditControlWidget::handleHttpResponse, this));
	(boost::bind(&BridgeEditControlWidget::connect, this));
	(boost::bind(&BridgeEditControlWidget::updateEdit, this));
}

//creates a client
Http::Client * BridgeEditControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handles post register requests and sets the bridge's registered status and user id
void BridgeEditControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		if (response.body().find("error") != -1) {
			update();
			errorText_->setText("Please link the bridge");
		}
		else {
			//Parse the username from the response JSON
			size_t pos = response.body().find("username");
			string substring = response.body().substr(pos + 11);
			size_t end = substring.find("\"");
			string username = substring.substr(0, end);


			//Get the bridge name, ip address, port number, and location
			string name = nameEdit_->text().toUTF8();
			string ip = ipEdit_->text().toUTF8();
			string port = portEdit_->text().toUTF8();
			string location = locationEdit_->text().toUTF8();

			//Creates a new bridge and adds it to the database
			Bridge *temp = new Bridge();
			temp->setBridgeName(name);
			temp->setIpAddress(ip);
			int x = stoi(port);
			temp->setPortNumber(x);
			temp->setUserId(username);
			temp->setLocation(location);


			//session_->UpdateBridge(temp);
			//session_->addBridgeUserId(temp, username);
			update();

		}
	}

}



void BridgeEditControlWidget::updateEdit() {
	errorText_->setText("");
	string portNum = portEdit_->text().toUTF8();
	string ipNum = ipEdit_->text().toUTF8();
	//Check if port is a number
	if (!port.empty() && find_if(port.begin(), port.end(), [](char c) { return !std::isdigit(c); }) == port.end()) {
		vector<Bridge> bridges = session_->getBridges();
		vector<int> ports;
		bool foundPort = false;
		//Creates a vector of all the port numbers
		for (size_t i = 0; i<bridges.size(); i++) {
			ports.push_back(bridges[i].getPortNumber());
		}

		//Check if the port number is registered yet
		for (size_t i = 0; i<ports.size(); i++) {
			if (ports[i] == stoi(port)) {
				foundPort = true;
			}
		}

		if (foundPort == false) {
			Http::Client *client = BridgeEditControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"devicetype\" : \"danny\"}");
			client->done().connect(boost::bind(&BridgeEditControlWidget::handleHttpResponse, this, _1, _2));
			client->post("http://" + ipNum + ":" + portNum + "/api", *msg);
		}
		else {
			errorText_->setText("That port number is already taken.");
		}
	}
	
	else {
		errorText_->setText("Please enter a number for the port.");
	}
}