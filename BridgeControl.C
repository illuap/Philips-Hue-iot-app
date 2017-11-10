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

#include "BridgeControl.h"
#include "Session.h"

using namespace Wt;
using namespace std;

string ipAddress = "ip";
string port = "port";

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
	/*user_ = new WText(this);
	user_->setText("test");*/

	//Database function to get the list of bridge ip addresses

	std::vector<Bridge> bridges = session_->getBridges();

	this->addWidget(new WText("Existing bridges:"));
	this->addWidget(new WBreak());
	for(int i = 0; i < bridges.size(); i++){
		this->addWidget(new WText(bridges[i].getBridgeName() + " " 
			+ bridges[i].getIpAddress() + ":" 
			+ std::to_string(bridges[i].getPortNumber())));
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
	

	this->addWidget(new WText("Enter a bridge name: "));
	name_ = new WLineEdit(this);
	name_->setMargin(5, Left);
	this->addWidget(new WBreak());

	ipList_ = new WText(this);
	//ipList_->setText(/*List of ip addresses*/);
	ip_ = new WText(this);
	ip_->setText("Enter the IP Address");
	ipEdit_ = new WLineEdit(this);
	ipEdit_->setMargin(5, Left);
	this->addWidget(new WBreak());

	port_ = new WText(this);
	port_->setText("Enter the port number");
	portEdit_ = new WLineEdit(this);
	portEdit_->setMargin(5, Left);
	this->addWidget(new WBreak());

	WPushButton *button
		= new WPushButton("Register", this);										
	button->setMargin(5, Left);

	this->addWidget(new WBreak());
	testText_ = new WText(this);
	testText_->setText("testing");
	WPushButton *showLightsButton= new WPushButton("Show Lights", this);	
	/*								
	Bridge *temp = new Bridge();
	temp->setIpAddress("911");
	session_->addBridge(temp);
	*/
	button->clicked().connect(this, &BridgeControlWidget::registerBridge);
	showLightsButton->clicked().connect(this, &BridgeControlWidget::showLights);
	(boost::bind(&BridgeControlWidget::registerBridge, this));
	(boost::bind(&BridgeControlWidget::handleHttpResponse, this));
	(boost::bind(&BridgeControlWidget::connect, this));
	(boost::bind(&BridgeControlWidget::showLights, this));

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
		
		//Get Bridge from database
		//Set registered status to true
		//Set userId to userId received from response

		ip = ipEdit_->text().toUTF8();
		port = portEdit_->text().toUTF8();

		/*Bridge* bridge = session_getBridge(ip+":"+port);
		bridge.setRegistered(true);
		bridge.setUserId(username);

		testText_->setText(bridge.getUserId);*/
		//clear();

	}
}

void BridgeControlWidget::registerBridge(){
	string name = name_->text().toUTF8();
	string ip = ipEdit_->text().toUTF8();
	string port = portEdit_->text().toUTF8();
	Http::Client *client = BridgeControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"devicetype\" : \"danny\"}");
	client->done().connect(boost::bind(&BridgeControlWidget::handleHttpResponse, this, _1, _2));
	client->post("http://"+ip+":"+port+"/api", *msg);
	testText_->setText(username+ip+port);

	Bridge *temp = new Bridge();

	temp->setBridgeName(name);
	temp->setIpAddress(ip);
	int x = stoi(port);
	temp->setPortNumber(x);
	temp->setUserId(username);

	session_->addBridge(temp);
}

void BridgeControlWidget::showLights() 
{
	clear();
	WApplication::instance()->setInternalPath("/lights?user="+username+"&ip="+ip+"&port="+port,  true);
 
}