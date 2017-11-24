// GroupsControl.C : Defines the GroupsControlWidget Application for creating and listing groups
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <boost/lexical_cast.hpp>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>
#include <Wt/WSlider>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Http/Client>
#include "GroupsControl.h"
#include "Session.h"


using namespace Wt;
using namespace std;

GroupsControlWidget::GroupsControlWidget(Session *session, WContainerWidget *parent) :
	WContainerWidget(parent),
	session_(session)
{
	setContentAlignment(AlignCenter);
	setStyleClass("highscores");
}

// Function Name: update()
// Parameters: none
// Return: none
// Description: generates the Widget
void GroupsControlWidget::update()
{
	clear();

	//get URL info
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("user=");								//get userID
	string subString = address.substr(pos + 5);
	size_t endPos = subString.find("&");
	userID = subString.substr(0, endPos);
	pos = address.find("ip=");										//get ip
	subString = address.substr(pos + 3);
	endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");									//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);

	one = false;
	two = false;
	three = false; 

	//return to lights page
	WPushButton *lightButton
		= new WPushButton("Return to My Lights", this);
	lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	lightButton->setMargin(10, Left);
	
	//return to bridge page
	WPushButton *returnButton						
		= new WPushButton("Return To Bridge", this);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//create a new group
	this->addWidget(new WText("CREATE A GROUP: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("Group name: "));
	nameEdit_ = new WLineEdit(this);												
	nameEdit_->setFocus();
	this->addWidget(new WBreak());

	//select the lights to be part of the group
	this->addWidget(new WText("Choose your lights: "));
	WPushButton *oneButton
		= new WPushButton("Light 1", this);                   // 1st light button
	oneButton->setMargin(5, Left);
	WPushButton *twoButton
		= new WPushButton("Light 2", this);                   // 2nd light button
	twoButton->setMargin(5, Left);
	WPushButton *threeButton
		= new WPushButton("Light 3", this);                   // 3rd light button
	threeButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	
	// displays which lights have been chosen
	this->addWidget(new WText("Selected lights for your new group: "));
	this->addWidget(new WBreak());
	light1_ = new WText(this);                          
	this->addWidget(new WBreak());
	light2_ = new WText(this);
	this->addWidget(new WBreak());
	light3_ = new WText(this);
	
	//create group
	this->addWidget(new WBreak());
	WPushButton *createButton
		= new WPushButton("Create Group", this);                  
	this->addWidget(new WBreak());

	status_ = new WText(this);                           //status of creating a group
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//list groups
	this->addWidget(new WText("Your Groups: "));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	groups_ = new WText(this);
	Http::Client *client = GroupsControlWidget::connect();
	client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/groups")) {
		WApplication::instance()->deferRendering();
	}

	oneButton->clicked().connect(this, &GroupsControlWidget::lightOne);
	twoButton->clicked().connect(this, &GroupsControlWidget::lightTwo);
	threeButton->clicked().connect(this, &GroupsControlWidget::lightThree);
	createButton->clicked().connect(this, &GroupsControlWidget::createGroup);
	returnButton->clicked().connect(this, &GroupsControlWidget::returnBridge);

	(boost::bind(&GroupsControlWidget::handleHttpResponse, this));
	(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&GroupsControlWidget::connect, this));
	(boost::bind(&GroupsControlWidget::lightOne, this));
	(boost::bind(&GroupsControlWidget::lightTwo, this));
	(boost::bind(&GroupsControlWidget::lightThree, this));
	(boost::bind(&GroupsControlWidget::createGroup, this));
	(boost::bind(&GroupsControlWidget::returnBridge, this));
}

// Function Name: connect() 
// Parameters: none
// Return: none
// Description: creates an Http client
Http::Client * GroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

// Function Name: handleHttpResponseVOID()
// Parameters: none
// Return: none
// Description: reloads the same Widget with changes to the group list
void GroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
	update();
}

// Function Name: handleHttpResponse()
// Parameters: none
// Return: none
// Description: displays the list of groups as buttons 
void GroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//find number of groups
		size_t pos = response.body().find_last_of("{");
		string subString = response.body().substr(pos - 3);
		size_t endPos = subString.find("\"");
		string num = subString.substr(0, endPos);
		int n = atoi(num.c_str());
		if (n <= 0) {
			n = 1;
		}

		//create a button for each group that leads to the ability to edit that specific group
		for (int i = 0; i < n; i++) {
			string groups = response.body();
			if (groups.find("\"" + to_string(i + 1) + "\":") != string::npos) {
				size_t pos = groups.find("\"" + to_string(i + 1) + "\"");
				string subString;
				if ( (i + 1) >= 10) {
					subString = groups.substr(pos + 14);
				} else {
					subString = groups.substr(pos + 13);
				}
				size_t endPos = subString.find("\"");
				string name = subString.substr(0, endPos);
				WPushButton *currentButton = new WPushButton(to_string(i + 1) + " - " + name, this);
				currentButton->setMargin(5, Left);
				currentButton->setLink("/?_=/singlegroup?user=" + userID + "%26ip=" + ip + "%26port=" + port + "%26groupid=" + to_string(i+1));
			}
		}
	}
}

// Function Name: lightOne()
// Parameters: none
// Return: none
// Description: selects light 1 to be added/removed as part of the new group
void GroupsControlWidget::lightOne() {
	if (!one) {
		one = true;
		light1_->setText("Light 1");
	} else {
		one = false;
		light1_->setText("");
	}
}

// Function Name: lightTwo()
// Parameters: none
// Return: none
// Description: selects light 2 to be added/removed as part of the new group
void GroupsControlWidget::lightTwo() {
	if (!two) {
		two = true;
		light2_->setText("Light 2");
	} else {
		two = false;
		light2_->setText("");
	}
}

// Function Name: lightThree()
// Parameters: none
// Return: none
// Description: selects light 3 to be added/removed as part of the new group
void GroupsControlWidget::lightThree() {
	if (!three) {
		three = true;
		light3_->setText("Light 3");
	} else {
		three = false;
		light3_->setText("");
	}
}

// Function Name: createGroup()
// Parameters: none
// Return: none
// Description: creates a new group
void GroupsControlWidget::createGroup() {
	//determine which lights have been chosen
	if (!one && !two && !three) {
		status_->setText("Select as least 1 light to be in your group");
	} else {
		if (nameEdit_->text().toUTF8() == "") {
			status_->setText("Enter a name for your group");
		} else {
			string selectedLights = "";
			if (one && two && three) {
				selectedLights = "[\"1\",\"2\",\"3\"]";
			} else {
				if (one && two) {
					selectedLights = "[\"1\",\"2\"]";
				} else {
					if (one && three) {
						selectedLights = "[\"1\",\"3\"]";
					} else {
						if (two && three) {
							selectedLights = "[\"2\",\"3\"]";
						} else {
							if (one) {
								selectedLights = "[\"1\"]";
							} else {
								if (two) {
									selectedLights = "[\"2\"]";
								} else {
									selectedLights = "[\"3\"]";
								}
							}
						}
					}
				}
			}
			status_->setText("Are you sure?");
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"lights\" : " + selectedLights + ", \"name\" : \"" + nameEdit_->text().toUTF8() + "\", \"type\" : \"LightGroup\" }");
			Http::Client *client = GroupsControlWidget::connect();
			client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
			client->post("http://" + ip + ":" + port + "/api/" + userID + "/groups", *msg);
		}
	}
}

// Function Name: returnBridge()
// Parameters: none
// Return: none
// Description: goes back to bridge page
void GroupsControlWidget::returnBridge() {
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
