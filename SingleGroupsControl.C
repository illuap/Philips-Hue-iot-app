/** @file SingleGroupsControl.C
*  @brief Application for deleting and altering states of individual groups
*  @author Nicole Chow
*  @author Weija Zhou
*  @author Paul Li
*  @author Daniel Le
*  @date Nov 28, 2017
*/

#include <unistd.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/WSound>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>
#include <Wt/WSlider>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Http/Client>
#include "SingleGroupsControl.h"
#include "Session.h"

using namespace Wt;
using namespace std;

SingleGroupsControlWidget::SingleGroupsControlWidget(Session *session, WContainerWidget *parent) :
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
void SingleGroupsControlWidget::update()
{
	clear();

	//get user info from URL
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
	pos = address.find("groupid=");									//get groupID
	subString = address.substr(pos + 8);
	endPos = subString.find("&");
	groupID = subString.substr(0, endPos);

	deleteConfirm = false;

	//get group info to display 
	Http::Client *client = SingleGroupsControlWidget::connect();
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID)) {
		WApplication::instance()->deferRendering();
	}
	groupInfoEdit_ = new WText(this);								//group name
	this->addWidget(new WBreak());
	groupLightsEdit_ = new WText(this);								//lights in the group
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change group name
	this->addWidget(new WText("Set New Group Name: "));
	nameEdit_ = new WLineEdit(this);												
	nameEdit_->setFocus();
	WPushButton *nameButton
		= new WPushButton("Change", this);										
	nameButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//preset light modes
	this->addWidget(new WText("Pre set light modes: "));
	WPushButton *sunsetModeButton
		= new WPushButton("Sunset Yellow", this);                     
	sunsetModeButton->setMargin(5, Left);
	WPushButton *oceanModeButton
		= new WPushButton("Ocean Blue", this);
	oceanModeButton->setMargin(5, Left);
	WPushButton *forestModeButton
		= new WPushButton("Forest Green", this);
	forestModeButton->setMargin(5, Left);
	WPushButton *bloodModeButton
		= new WPushButton("Blood Red", this);
	bloodModeButton->setMargin(5, Left);
	WPushButton *mustangModeButton
		= new WPushButton("Mustang Purple", this);
	mustangModeButton->setMargin(5, Left);
	WPushButton *fireModeButton
		= new WPushButton("Fire Orange", this);
	fireModeButton->setMargin(5, Left);
	WPushButton *fiftyModeButton
		= new WPushButton("50 Shades", this);
	fiftyModeButton->setMargin(5, Left);
	WPushButton *partyModeButton
		= new WPushButton("Party Mode w. Music (10s duration)", this);                    
	partyModeButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//turn on
	this->addWidget(new WText("Light on/off: "));
	WPushButton *onButton
		= new WPushButton("ON", this);                      // ON button
	onButton->setMargin(5, Left);

	//turn off
	WPushButton *offButton
		= new WPushButton("OFF", this);                     // OFF button
	offButton->setMargin(5, Left);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change hue
	this->addWidget(new WText("Hue: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("0  "));
	hueScaleSlider_ = new WSlider(this);					 //slider bar
	hueScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	hueScaleSlider_->setMinimum(0);
	hueScaleSlider_->setMaximum(65535);
	hueScaleSlider_->setValue(100);
	hueScaleSlider_->setTickInterval(10000);
	hueScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	hueScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  65535"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change brightness
	this->addWidget(new WText("Brightness: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("1  "));
	briScaleSlider_ = new WSlider(this);					
	briScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	briScaleSlider_->setMinimum(1);
	briScaleSlider_->setMaximum(254);
	briScaleSlider_->setValue(100);
	briScaleSlider_->setTickInterval(50);
	briScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	briScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  254"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change saturation
	this->addWidget(new WText("Saturation: "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("0  "));
	satScaleSlider_ = new WSlider(this);				
	satScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	satScaleSlider_->setMinimum(0);
	satScaleSlider_->setMaximum(254);
	satScaleSlider_->setValue(100);
	satScaleSlider_->setTickInterval(50);
	satScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	satScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  254"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change transition time
	this->addWidget(new WText("Transition Time: (multiple of 100ms) "));
	this->addWidget(new WBreak());
	this->addWidget(new WText("1  (100ms)"));
	transitionScaleSlider_ = new WSlider(this);					
	transitionScaleSlider_->setOrientation(Wt::Orientation::Horizontal);
	transitionScaleSlider_->setMinimum(1);
	transitionScaleSlider_->setMaximum(20);
	transitionScaleSlider_->setValue(4);
	transitionScaleSlider_->setTickInterval(2);
	transitionScaleSlider_->setTickPosition(Wt::WSlider::TicksBothSides);
	transitionScaleSlider_->resize(300, 50);
	this->addWidget(new WText("  20 (2 seconds)"));
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//add lights
	this->addWidget(new WText("Add Light: "));
	addChoices_ = new WComboBox(this);											
	WPushButton *addButton
		= new WPushButton("Add", this);										// submit button
	addButton->setMargin(5, Left);
	addChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//remove lights
	this->addWidget(new WText("Remove Light: "));
	removeChoices_ = new WComboBox(this);												
	WPushButton *removeButton
		= new WPushButton("Remove", this);										
	removeButton->setMargin(5, Left);
	removeChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	
	//delete group
	WPushButton *deleteButton							
		= new WPushButton("Delete This Group", this);

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	change_ = new WText(this);                          //status of a light change
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//return to groups page
	WPushButton *groupButton
		= new WPushButton("Return to My Groups", this);
	groupButton->setLink("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	groupButton->setMargin(10, Left);
	
	//return to lights page
	WPushButton *lightButton
		= new WPushButton("Return to My Lights", this);
	lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	lightButton->setMargin(10, Left);
	WPushButton *returnButton							
		= new WPushButton("Return To Bridge", this);

	onButton->clicked().connect(this, &SingleGroupsControlWidget::on);
	partyModeButton->clicked().connect(this, &SingleGroupsControlWidget::partyMode);
	mustangModeButton->clicked().connect(this, &SingleGroupsControlWidget::mustangMode);
	oceanModeButton->clicked().connect(this, &SingleGroupsControlWidget::oceanMode);
	bloodModeButton->clicked().connect(this, &SingleGroupsControlWidget::bloodMode);
	fireModeButton->clicked().connect(this, &SingleGroupsControlWidget::fireMode);
	fiftyModeButton->clicked().connect(this, &SingleGroupsControlWidget::fiftyMode);
	forestModeButton->clicked().connect(this, &SingleGroupsControlWidget::forestMode);
	sunsetModeButton->clicked().connect(this, &SingleGroupsControlWidget::sunsetMode);
	addButton->clicked().connect(this, &SingleGroupsControlWidget::addLights);
	removeButton->clicked().connect(this, &SingleGroupsControlWidget::removeLights);
	nameButton->clicked().connect(this, &SingleGroupsControlWidget::name);
	offButton->clicked().connect(this, &SingleGroupsControlWidget::off);
	returnButton->clicked().connect(this, &SingleGroupsControlWidget::returnBridge);
	deleteButton->clicked().connect(this, &SingleGroupsControlWidget::deleteGroup);
	briScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::bright);
	satScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::sat);
	hueScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::hue);
	transitionScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::transition);


	(boost::bind(&SingleGroupsControlWidget::hue, this));
	(boost::bind(&SingleGroupsControlWidget::name, this));
	(boost::bind(&SingleGroupsControlWidget::bright, this));
	(boost::bind(&SingleGroupsControlWidget::sat, this));
	(boost::bind(&SingleGroupsControlWidget::on, this));
	(boost::bind(&SingleGroupsControlWidget::off, this));
	(boost::bind(&SingleGroupsControlWidget::returnBridge, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponse, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this));
	(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&SingleGroupsControlWidget::connect, this));
	(boost::bind(&SingleGroupsControlWidget::deleteGroup, this));
	(boost::bind(&SingleGroupsControlWidget::addLights, this));
	(boost::bind(&SingleGroupsControlWidget::removeLights, this));
	(boost::bind(&SingleGroupsControlWidget::transition, this));
	(boost::bind(&SingleGroupsControlWidget::partyMode, this));
	(boost::bind(&SingleGroupsControlWidget::sunsetMode, this));
	(boost::bind(&SingleGroupsControlWidget::bloodMode, this));
	(boost::bind(&SingleGroupsControlWidget::oceanMode, this));
	(boost::bind(&SingleGroupsControlWidget::forestMode, this));
	(boost::bind(&SingleGroupsControlWidget::fireMode, this));
	(boost::bind(&SingleGroupsControlWidget::fiftyMode, this));
	(boost::bind(&SingleGroupsControlWidget::mustangMode, this));
}

// Function Name: connect() 
// Parameters: none
// Return: none
// Description: creates an Http client
Http::Client * SingleGroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

// Function Name: handleHttpResponseUpdate()
// Parameters: none
// Return: none
// Description: reloads the same Widget with changes to the group (for name/light member changes)
void SingleGroupsControlWidget::handleHttpResponseUpdate(boost::system::error_code err, const Http::Message& response) {
	update();
}

// Function Name: handleHttpResponseVOID()
// Parameters: none
// Return: none
// Description: empty function for responses that don't need to be processed (for on/off/hue/sat/bri/transition changes)
void SingleGroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

// Function Name: handleHttpResponse()
// Parameters: none
// Return: none
// Description: parses and displays group information when update() is called
void SingleGroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//get group name and lights in the group
		Json::Object result;
		Json::parse(response.body(), result);
		string name = result.get("name");
		size_t pos = response.body().find("lights");
		string subString = response.body().substr(pos + 10);
		size_t endPos = subString.find("]");
		lights = subString.substr(0, endPos);
		boost::erase_all(lights, "\"");
		groupInfoEdit_->setText("Group Name: " + name);
		groupLightsEdit_->setText("Lights in your Group: " + lights);
		removeChoices_->clear();
		addChoices_->clear();

		//give user choices to add/remove lights
		if (lights.find("1") != string::npos) {
			removeChoices_->addItem("1");
		} else {
			addChoices_->addItem("1");
		}

		if (lights.find("2") != string::npos) {
			removeChoices_->addItem("2");
		} else {
			addChoices_->addItem("2");
		}

		if (lights.find("3") != string::npos) {
			removeChoices_->addItem("3");
		} else {
			addChoices_->addItem("3");
		}

		change_->setText("");
	}
}

// Function Name: deleteGroup()
// Parameters: none
// Return: none
// Description: Deletes a group
void SingleGroupsControlWidget::deleteGroup() {

	if (!deleteConfirm) {
		change_->setText("You are about to delete this group. Are you sure?");
		deleteConfirm = true;
	} else {
		Http::Message *msg = new Http::Message();
		Http::Client *client = SingleGroupsControlWidget::connect();
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->deleteRequest("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
		returnBridge();
	}
}

// Function Name: addLights()
// Parameters: none
// Return: none
// Description: adds a light to a group
void SingleGroupsControlWidget::addLights() {
	bool one = false;
	bool two = false;
	bool three = false;
	if (addChoices_->currentText() == "1" || addChoices_->currentText() == "2" || addChoices_->currentText() == "3") {
		//get lights already in the group
		if (lights.find("1") != string::npos) {
			one = true;
		}
		if (lights.find("2") != string::npos) {
			two = true;
		}
		if (lights.find("3") != string::npos) {
			three = true;
		}

		//get light that needs to be added
		if (addChoices_->currentText() == "1") {
			one = true;
		} else {
			if (addChoices_->currentText() == "2") {
				two = true;
			} else {
				three = true;
			}
		}

		//create body message (lights already in group + light that needs to be added)
		string selectedLights;
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

		change_->setText("Are you sure?");
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"lights\" : " + selectedLights + "}");
		Http::Client *client = SingleGroupsControlWidget::connect();
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
	} else {
		change_->setText("Please choose a light. If there are no choices then all lights are already added.");
	}
}

// Function Name: removeLights()
// Parameters: none
// Return: none
// Description: removes a light from a group
void SingleGroupsControlWidget::removeLights() {
	bool one = false;
	bool two = false;
	bool three = false;
	if (removeChoices_->currentText() == "1" || removeChoices_->currentText() == "2" || removeChoices_->currentText() == "3") {
		//get lights already in the group
		if (lights.find("1") != string::npos) {
			one = true;
		}
		if (lights.find("2") != string::npos) {
			two = true;
		}
		if (lights.find("3") != string::npos) {
			three = true;
		}

		//get light that needs to be removed
		if (removeChoices_->currentText() == "1") {
			one = false;
		} else {
			if (removeChoices_->currentText() == "2") {
				two = false;
			} else {
				three = false;
			}
		}

		//if there is only 1 light in the group, it can't be deleted
		if (!three && !two && !one) {
			change_->setText("You must have at least 1 light in your group");
		} else {
			//create body message (lights already in group - light that needs to be removed)
			string selectedLights;
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
			change_->setText("Are you sure?");
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"lights\" : " + selectedLights + "}");
			Http::Client *client = SingleGroupsControlWidget::connect();
			client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
			client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
		}
	} else {
		change_->setText("Please choose a light. If there are no choices then you cannot remove any lights (groups must have at least 1 light)");
	}

}

// Function Name: name()
// Parameters: none
// Return: none
// Description: changes the group name
void SingleGroupsControlWidget::name() {
	string input = nameEdit_->text().toUTF8();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"name\" : \"" + input + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
	change_->setText("Are you sure?");
}

// Function Name: on()
// Parameters: none
// Return: none
// Description: turns lights on
void SingleGroupsControlWidget::on() {
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : true}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: ON");
}

// Function Name: off()
// Parameters: none
// Return: none
// Description: turns lights off
void SingleGroupsControlWidget::off() {
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : false}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: OFF");
}

// Function Name: hue()
// Parameters: none
// Return: none
// Description: changes group's hue
void SingleGroupsControlWidget::hue() {
	int input = hueScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"hue\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Hue: " + to_string(input));
}

// Function Name: bright()
// Parameters: none
// Return: none
// Description: changes group's brightness
void SingleGroupsControlWidget::bright() {
	int input = briScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"bri\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Brightness: " + to_string(input));
}

// Function Name: sat()
// Parameters: none
// Return: none
// Description: changes group's saturation
void SingleGroupsControlWidget::sat(){
	int input = satScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"sat\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Saturation: " + to_string(input));
}

// Function Name: transition()
// Parameters: none
// Return: none
// Description: changes group's transition time
void SingleGroupsControlWidget::transition() {
	int input = transitionScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"transitiontime\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Transition Time: " + to_string(input * 100) + "ms");
}

// Function Name: sunsetMode()
// Parameters: none
// Return: none
// Description: turns on sunset yellow mode
void SingleGroupsControlWidget::sunsetMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"103\" , \"hue\" : \"10532\" , \"bri\" : \"211\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"59\" , \"hue\" : \"8894\" , \"bri\" : \"249\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"203\" , \"hue\" : \"10064\" , \"bri\" : \"184\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Sunset Yellow");
}

// Function Name: partyMode()
// Parameters: none
// Return: none
// Description: turns on party mode for 10s
void SingleGroupsControlWidget::partyMode() {
	//create child to play music in the background
	pid_t pid = fork();
	if (pid == 0) {
		system("aplay party.wav");
		exit(0);
	} else {
		change_->setText("!PARTY MODE! Turn on sound for music. (Please wait until party mode completes to continue)");
		for (int i = 0; i < 40; i++) {
			if (i % 5 == 0) {
				if (lights.find("1") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"14043\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
				}
				if (lights.find("2") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"55237\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
				}
				if (lights.find("3") != string::npos) {
					Http::Client *client = SingleGroupsControlWidget::connect();
					Http::Message *msg = new Http::Message();
					msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"9596\" , \"bri\" : \"254\"}");
					client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
					client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
				}

			}
			else {
				if (i % 4 == 0) {
					if (lights.find("1") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"49619\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
					}
					if (lights.find("2") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"19192\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
					}
					if (lights.find("3") != string::npos) {
						Http::Client *client = SingleGroupsControlWidget::connect();
						Http::Message *msg = new Http::Message();
						msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"42364\" , \"bri\" : \"254\"}");
						client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
						client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
					}
				}
				else {
					if (i % 3 == 0) {
						if (lights.find("1") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"8192\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
						}
						if (lights.find("2") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"36278\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
						}
						if (lights.find("3") != string::npos) {
							Http::Client *client = SingleGroupsControlWidget::connect();
							Http::Message *msg = new Http::Message();
							msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"60620\" , \"bri\" : \"254\"}");
							client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
							client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
						}
					}
					else {
						if (i % 2 == 0) {
							if (lights.find("1") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"32299\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
							}
							if (lights.find("2") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"65535\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
							}
							if (lights.find("3") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"27384\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
							}
						}
						else {
							if (lights.find("1") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"13107\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
							}
							if (lights.find("2") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"56407\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
							}
							if (lights.find("3") != string::npos) {
								Http::Client *client = SingleGroupsControlWidget::connect();
								Http::Message *msg = new Http::Message();
								msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"49151\" , \"bri\" : \"254\"}");
								client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
								client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
							}
						}
					}
				}
			}
			usleep(250000);
		}
	}
	change_->setText("");
}

// Function Name: fiftyMode()
// Parameters: none
// Return: none
// Description: turns on 50 Shades mode
void SingleGroupsControlWidget::fiftyMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"1\" , \"hue\" : \"26214\" , \"bri\" : \"211\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"58\" , \"hue\" : \"48215\" , \"bri\" : \"82\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"0\" , \"hue\" : \"60620\" , \"bri\" : \"173\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: 50 Shades");
}

// Function Name: oceanMode()
// Parameters: none
// Return: none
// Description: turns on ocean blue mode
void SingleGroupsControlWidget::oceanMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"191\" , \"hue\" : \"33236\" , \"bri\" : \"212\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"188\" , \"hue\" : \"42364\" , \"bri\" : \"169\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"35810\" , \"bri\" : \"247\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Ocean Blue");
}

// Function Name: forestMode()
// Parameters: none
// Return: none
// Description: turns on forest green mode
void SingleGroupsControlWidget::forestMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"23873\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"102\" , \"hue\" : \"29023\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"121\" , \"hue\" : \"16618\" , \"bri\" : \"150\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Forest Green");
}

// Function Name: mustangMode()
// Parameters: none
// Return: none
// Description: turns on mustang purple mode
void SingleGroupsControlWidget::mustangMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"82\" , \"hue\" : \"45874\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"152\" , \"hue\" : \"51726\" , \"bri\" : \"109\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"50322\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Mustang Purple");
}

// Function Name: fireMode()
// Parameters: none
// Return: none
// Description: turns on fire orange mode
void SingleGroupsControlWidget::fireMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"7256\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"227\" , \"hue\" : \"3511\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"147\" , \"hue\" : \"7490\" , \"bri\" : \"212\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Fire Orange");
}

// Function Name: bloodMode()
// Parameters: none
// Return: none
// Description: turns on blood red mode
void SingleGroupsControlWidget::bloodMode() {
	if (lights.find("1") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"132\" , \"hue\" : \"0\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/1/state", *msg);
	}
	if (lights.find("2") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"3511\" , \"bri\" : \"161\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/2/state", *msg);
	}
	if (lights.find("3") != string::npos) {
		Http::Client *client = SingleGroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true , \"sat\" : \"254\" , \"hue\" : \"63663\" , \"bri\" : \"254\"}");
		client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/3/state", *msg);
	}
	change_->setText("Mode: Blood Red");
}

// Function Name: returnBridge()
// Parameters: none
// Return: none
// Description: goes back to bridge page
void SingleGroupsControlWidget::returnBridge(){
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
