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

void GroupsControlWidget::update()
{
	clear();

	/*
	//get URL info
	string address = WApplication::instance()->internalPath();
	size_t pos = address.find("user=");						//get userID
	string subString = address.substr(pos + 5);
	size_t endPos = subString.find("&");
	userID = subString.substr(0, endPos);
	pos = address.find("ip=");								//get ip
	subString = address.substr(pos + 3);
	endPos = subString.find("&");
	ip = subString.substr(0, endPos);
	pos = address.find("port=");								//get port
	subString = address.substr(pos + 5);
	endPos = subString.find("&");
	port = subString.substr(0, endPos);
	*/

	one = false;
	two = false;
	three = false; 

	this->addWidget(new WText("CREATE A GROUP: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("Group name: "));
	nameEdit_ = new WLineEdit(this);												//input for group name
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

	status_ = new WText(this);                           // displays status of creating a group
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//list groups
	this->addWidget(new WText("Your Groups: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("Note: \"1- Group 1\" is a default group containing all your lights."));
	this->addWidget(new WBreak());
	groups_ = new WText(this);
	Http::Client *client = GroupsControlWidget::connect();
	client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://127.0.0.1:8000/api/newdeveloper/groups")) {
		WApplication::instance()->deferRendering();
	}

	oneButton->clicked().connect(this, &GroupsControlWidget::lightOne);
	twoButton->clicked().connect(this, &GroupsControlWidget::lightTwo);
	threeButton->clicked().connect(this, &GroupsControlWidget::lightThree);
	createButton->clicked().connect(this, &GroupsControlWidget::createGroup);

	(boost::bind(&GroupsControlWidget::handleHttpResponse, this));
	(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&GroupsControlWidget::connect, this));
	(boost::bind(&GroupsControlWidget::lightOne, this));
	(boost::bind(&GroupsControlWidget::lightTwo, this));
	(boost::bind(&GroupsControlWidget::lightThree, this));
	(boost::bind(&GroupsControlWidget::createGroup, this));
}

//creates a client
Http::Client * GroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handle request (does nothing withthe response) - for creating a group
void GroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
	update();
}

//handles get groups
void GroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//find number of groups
		size_t pos = response.body().find_last_of(",");
		string subString = response.body().substr(pos + 2);
		size_t endPos = subString.find("\"");
		string num = subString.substr(0, endPos);
		int n = atoi(num.c_str());
		if (n == NULL || n <= 0) {
			n = 1;
		}
		for (int i = 0; i < n; i++) {
			string groups = response.body();
			if (groups.find("\"" + to_string(i + 1) + "\"") != string::npos) {
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
				//currentButton->setLink("/?_=/lights?user=" + x.getUserId() + "%26ip=" + x.getIpAddress() + "%26port=" + std::to_string(x.getPortNumber()));
			}
		}
	}
}

//selects light 1
void GroupsControlWidget::lightOne() {
	if (!one) {
		one = true;
		light1_->setText("Light 1");
	} else {
		one = false;
		light1_->setText("");
	}
}

//selects light 2
void GroupsControlWidget::lightTwo() {
	if (!two) {
		two = true;
		light2_->setText("Light 2");
	} else {
		two = false;
		light2_->setText("");
	}
}

//selects light 3
void GroupsControlWidget::lightThree() {
	if (!three) {
		three = true;
		light3_->setText("Light 3");
	} else {
		three = false;
		light3_->setText("");
	}
}

//creates group
void GroupsControlWidget::createGroup() {
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
			client->post("http://127.0.0.1:8000/api/newdeveloper/groups", *msg);
		}
	}
}
