#include <stdio.h>
#include <iostream>

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

	//
	//Database function to get the list of bridge ip addresses
	//Code combining these ip addresses into a single string
	//

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
	/*								
	Bridge *temp = new Bridge();
	temp->setIpAddress("911");
	session_->addBridge(temp);
	*/
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
		string username = substring.substr(0,end);
		
		//Get Bridge from database
		//Set registered status to true
		//Set userId to userId received from response

		string ip = ipEdit_->text().toUTF8();
		string port = portEdit_->text().toUTF8();

		//
		//Get bridge from database who has the same ip address and port as those entered by the user
		//Set registered status to true
		//Set user id as the username variable
		//


		/*Bridge* bridge = session_getBridge(ip+":"+port);
		bridge.setRegistered(true);
		bridge.setUserId(username);

		testText_->setText(bridge.getUserId);*/
		//clear();

	}
}

void BridgeControlWidget::registerBridge(){
	string ip = ipEdit_->text().toUTF8();
	string port = portEdit_->text().toUTF8();
	Http::Client *client = BridgeControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"devicetype\" : \"danny\"}");
	client->done().connect(boost::bind(&BridgeControlWidget::handleHttpResponse, this, _1, _2));
	client->post("http://"+ip+":"+port+"/api", *msg);
}