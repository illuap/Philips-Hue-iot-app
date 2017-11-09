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
	WPushButton *button
		= new WPushButton("Register", this);										// 1st light button
	button->setMargin(5, Left);
	this ->addWidget(new WBreak());
	user_ = new WText(this);
	user_->setText("test");
													

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

//handles get lights request - creates lights and puts them into database
void BridgeControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//Parse the username from the response JSON
		size_t pos = response.body().find("username");
		string substring = response.body().substr(pos+11);
		size_t end = substring.find("\"");
		string username = substring.substr(0,end);
		//Test output. Remove later
		user_->setText(username);
		
		
		//Get Bridge from database
		//Set registered status to true
		//Set userId to userId received from response

		/*Bridge* bridge = new Bridge();
		bridge.setRegistered(true);
		bridge.setUserId(username);*/
	}
}



void BridgeControlWidget::registerBridge(){
	Http::Client *client = BridgeControlWidget::connect();
		 Http::Message *msg = new Http::Message();
		 msg->addBodyText("{\"devicetype\" : \"danny\"}");
		 client->done().connect(boost::bind(&BridgeControlWidget::handleHttpResponse, this, _1, _2));
		 client->post("http://localhost:8000/api", *msg);
		 

}