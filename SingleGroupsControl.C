#include <unistd.h>
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

void SingleGroupsControlWidget::update()
{
	clear();

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
	pos = address.find("groupid=");								//get groupID
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

	groupInfoEdit_ = new WText(this);
	this->addWidget(new WBreak());
	groupLightsEdit_ = new WText(this);
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//change name
	this->addWidget(new WText("Set New Group Name: "));
	nameEdit_ = new WLineEdit(this);												// user text input
	nameEdit_->setFocus();
	WPushButton *nameButton
		= new WPushButton("Change", this);										// submit button
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
		= new WPushButton("Party Mode (10s duration)", this);                    
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
	briScaleSlider_ = new WSlider(this);					 //slider bar
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
	satScaleSlider_ = new WSlider(this);					//slider bar
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
	transitionScaleSlider_ = new WSlider(this);					 //slider bar
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
	addChoices_ = new WComboBox(this);												// user text input
	WPushButton *addButton
		= new WPushButton("Add", this);										// submit button
	addButton->setMargin(5, Left);
	addChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	//remove lights
	this->addWidget(new WText("Remove Light: "));
	removeChoices_ = new WComboBox(this);												// user text input
	WPushButton *removeButton
		= new WPushButton("Remove", this);										// submit button
	removeButton->setMargin(5, Left);
	removeChoices_->clear();
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	
	WPushButton *deleteButton							//delete group
		= new WPushButton("Delete This Group", this);

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	change_ = new WText(this);                          //displays the status of a light change
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	this->addWidget(new WBreak());
	this->addWidget(new WBreak());

	WPushButton *groupButton
		= new WPushButton("Return to My Groups", this);
	groupButton->setLink("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	groupButton->setMargin(10, Left);
	WPushButton *lightButton
		= new WPushButton("Return to My Lights", this);
	lightButton->setLink("/?_=/lights?user=" + userID + "%26ip=" + ip + "%26port=" + port);
	lightButton->setMargin(10, Left);
	WPushButton *returnButton							//go back to bridge
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

//creates a client
Http::Client * SingleGroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handle request (does nothing with the response) - for changing a group state
void SingleGroupsControlWidget::handleHttpResponseUpdate(boost::system::error_code err, const Http::Message& response) {
	update();
}

//handle request (does nothing withthe response) - for changing a group state
void SingleGroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

//handles get group state: gives info about the group's name and lights in the group 
void SingleGroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
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

		//create body message
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

		if (!three && !two && !one) {
			change_->setText("You must have at least 1 light in your group");
		} else {
			//create body message
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

//changes the name
void SingleGroupsControlWidget::name() {
	string input = nameEdit_->text().toUTF8();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"name\" : \"" + input + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseUpdate, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID, *msg);
	change_->setText("Are you sure?");
}


//turns light on
void SingleGroupsControlWidget::on() {
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : true}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: ON");
}

//turns light off
void SingleGroupsControlWidget::off() {
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : false}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("Light: OFF");
}

//changes the hue 
void SingleGroupsControlWidget::hue() {
	int input = hueScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"hue\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Hue: " + to_string(input));
}

//changes the brightness
void SingleGroupsControlWidget::bright() {
	int input = briScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"bri\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Brightness: " + to_string(input));
}

//changes the saturation
void SingleGroupsControlWidget::sat(){
	int input = satScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"sat\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Saturation: " + to_string(input));
}

//changes the transition
void SingleGroupsControlWidget::transition() {
	int input = transitionScaleSlider_->value();
	Http::Client *client = SingleGroupsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"transitiontime\" : \"" + to_string(input) + "\"}");
	client->done().connect(boost::bind(&SingleGroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
	client->put("http://" + ip + ":" + port + "/api/" + userID + "/groups/" + groupID + "/action", *msg);
	change_->setText("new Transition Time: " + to_string(input * 100) + "ms");
}

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

void SingleGroupsControlWidget::partyMode() {
	change_->setText("!PARTY MODE! Turn on sound for music. (Please wait until party mode completes to continue)");
	WSound *sound = new WSound("party.mp3");
	sound->play();
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

		} else {
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
			} else {
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
				} else {
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
					} else {
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
	change_->setText("");
	sound->stop();
}

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


void SingleGroupsControlWidget::returnBridge(){
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
