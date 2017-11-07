/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/lexical_cast.hpp>

#include <Wt/WText>
#include <Wt/WTable>
#include <Wt/Dbo/Dbo>
#include <Wt/WPushButton>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/Http/Message>
#include <Wt/WApplication>

#include <Wt/Http/Client>
#include "LightsControl.h"
#include "Session.h"


using namespace Wt;
using namespace std; 

LightsControlWidget::LightsControlWidget(Session *session, WContainerWidget *parent):
  WContainerWidget(parent),
  session_(session)
{
  setContentAlignment(AlignCenter);
  setStyleClass("highscores");
}

void LightsControlWidget::update()
{
  clear();
  for (int i = 1; i < 4; i++){
    Http::Client *client = LightsControlWidget::connect();
    client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
    if (client->get("http://localhost:8000/api/newdeveloper/lights/" + to_string(i))) {
      WApplication::instance()->deferRendering();
    }
  }
  
  new WText("Select the light to be changed: "); 
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
  this->addWidget(new WBreak());

  //turn on
  this->addWidget(new WText("Light on/off: "));
  WPushButton *onButton
    = new WPushButton("ON", this);                      // ON button
  onButton->setMargin(5, Left);                         // add 5 pixels margin

                                          //turn off
  WPushButton *offButton
    = new WPushButton("OFF", this);                     // OFF button
  offButton->setMargin(5, Left);                          // add 5 pixels margin
  this->addWidget(new WBreak());                        // insert a line break
  this->addWidget(new WBreak());

  //change hue
  this->addWidget(new WText("Hue: "));
  hueEdit_ = new WLineEdit(this);                       // user text input
  hueEdit_->setFocus();                             // give focus
  WPushButton *hueButton
    = new WPushButton("Change", this);                    // submit button
  hueButton->setMargin(5, Left);                          // add 5 pixels margin
  this->addWidget(new WBreak());                        // insert a line break
  this->addWidget(new WBreak());

  //change brightness
  this->addWidget(new WText("Brightness: "));
  briEdit_ = new WLineEdit(this);                       // user text input
  briEdit_->setFocus();                             // give focus
  WPushButton *brightButton
    = new WPushButton("Change", this);                    // submit button
  brightButton->setMargin(5, Left);                       // add 5 pixels margin
  this->addWidget(new WBreak());                        // insert a line break
  this->addWidget(new WBreak());

  //change saturation
  this->addWidget(new WText("Saturation: "));
  satEdit_ = new WLineEdit(this);                       // user text input
  satEdit_->setFocus();                             // give focus
  WPushButton *satButton
    = new WPushButton("Change", this);                    // submit button
  satButton->setMargin(5, Left);                          // add 5 pixels margin
  this->addWidget(new WBreak());                        // insert a line break
  this->addWidget(new WBreak());


  light_ = new WText(this);                           // displays which light is being changed
  this->addWidget(new WBreak());
  change_ = new WText(this);                          //displays the status of a light change


  hueButton->clicked().connect(this, &LightsControlWidget::hue);
  brightButton->clicked().connect(this, &LightsControlWidget::bright);
  satButton->clicked().connect(this, &LightsControlWidget::sat);
  onButton->clicked().connect(this, &LightsControlWidget::on);
  offButton->clicked().connect(this, &LightsControlWidget::off);
  oneButton->clicked().connect(this, &LightsControlWidget::lightOne);
  twoButton->clicked().connect(this, &LightsControlWidget::lightTwo);
  threeButton->clicked().connect(this, &LightsControlWidget::lightThree);

  (boost::bind(&LightsControlWidget::hue, this));
  (boost::bind(&LightsControlWidget::bright, this));
  (boost::bind(&LightsControlWidget::sat, this));
  (boost::bind(&LightsControlWidget::on, this));
  (boost::bind(&LightsControlWidget::off, this));
  (boost::bind(&LightsControlWidget::handleHttpResponse, this));
  (boost::bind(&LightsControlWidget::connect, this));
  (boost::bind(&LightsControlWidget::lightOne, this));
  (boost::bind(&LightsControlWidget::lightTwo, this));
  (boost::bind(&LightsControlWidget::lightThree, this));
}



//creates a client
Http::Client * LightsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handles get lights request - creates lights and puts them into database
void LightsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		Json::Object result;
		Json::parse(response.body(), result);

		//get name
		string name = result.get("name");

		//get type
		string type = result.get("type");

		//get sat
		size_t pos = response.body().find("sat");
		string subString = response.body().substr(pos + 5);
		size_t endPos = subString.find(",");
		string sat = subString.substr(0, endPos);

		//get bri
		pos = response.body().find("bri");
		subString = response.body().substr(pos + 5);
		endPos = subString.find(",");
		string bri = subString.substr(0, endPos);

		//get hue
		pos = response.body().find("hue");
		subString = response.body().substr(pos + 5);
		endPos = subString.find(",");
		string hue = subString.substr(0, endPos);

		//get on
		pos = response.body().find("\"on\"");
		subString = response.body().substr(pos + 5);
		endPos = subString.find("}");
		string on = subString.substr(0, endPos);
		bool lightOn = false;
		if (on.compare("true") == 0) {
			lightOn = true;
		}

		//Create light instance and add to database
		Light *x = new Light(name, type, stoi(bri), stoi(hue), stoi(sat), lightOn, 0);
		session_->addLight(x);
	}
}

//selects light 1 to change
void LightsControlWidget::lightOne() {
	currentLight = "1";
	light_->setText("You are changing Light 1");
	change_->setText("");
}

//selects light 2 to change
void LightsControlWidget::lightTwo() {
	currentLight = "2";
	light_->setText("You are changing Light 2");
	change_->setText("");

}

//selects light 3 to change
void LightsControlWidget::lightThree() {
	currentLight = "3";
	light_->setText("You are changing Light 3");
	change_->setText("");
}

//changes the hue 
void LightsControlWidget::hue() {
	change_->setText("");
	//get user input and check if it is a positive integer
	std::string input = hueEdit_->text().toUTF8();
	bool num = true;
	for (int i = 0; i < input.length(); i++) {
		if (!(isdigit(input[i]))) {
			num = false;
		}
	}

	//change hue if input is a valid hue input
	int value = atoi(input.c_str());
	if ((!num) || (value < 0) || (value > 65535)) {
		change_->setText("Please input integer between 0 and 65535 to change the hue");
	}
	else {
		if (currentLight.compare("0") == 0) {
			light_->setText("Please select a light to change");
		}
		else {
			Http::Client *client = LightsControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"hue\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Hue has been changed");

			// CHANGE DB ENTRY
			Light *x;
			x = session_->getLight("Hue Lamp " + currentLight);
			x->setHue(stoi(input));
			session_->updateLight(x);

			//test db changes
			x = session_->getLight("Hue Lamp " + currentLight);
			change_->setText("new Hue: " + to_string(x->getHue()));

		}
	}
}


//turns light on
void LightsControlWidget::on() {
	change_->setText("");
	Http::Client *client = LightsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : true}");
	client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
		change_->setText("Light has been turned on");

		//CHANGE DB ENTRY
		Light *x;
		x = session_->getLight("Hue Lamp " + currentLight);
		x->setOn(true);
		session_->updateLight(x);
		

		//test db changes
		x = session_->getLight("Hue Lamp " + currentLight);
		if (x->getOn()) {
			change_->setText("Light: ON");
		} else {
			change_->setText("Light: OFF");
		}
	}
}

//turns light off
void LightsControlWidget::off() {
	change_->setText("");
	Http::Client *client = LightsControlWidget::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : false}");
	client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
		change_->setText("Light has been turned off");

		//CHANGE DB ENTRY
		Light *x;
		x = session_->getLight("Hue Lamp " + currentLight);
		x->setOn(false);
		session_->updateLight(x);

		//test db changes
		x = session_->getLight("Hue Lamp " + currentLight);
		if (x->getOn()) {
			change_->setText("Light: ON");
		} else {
			change_->setText("Light: OFF");
		}

	}
}

//changes the brightness
void LightsControlWidget::bright() {
	change_->setText("");
	//get user input and check if it is a positive integer
	std::string input = briEdit_->text().toUTF8();
	bool num = true;
	for (int i = 0; i < input.length(); i++) {
		if (!(isdigit(input[i]))) {
			num = false;
		}
	}

	//change brightness if input is a valid brightness input
	int value = atoi(input.c_str());
	if ((!num) || (value < 1) || (value > 254)) {
		change_->setText("Please input integer between 1 and 254 to change the brightness");
	}
	else {
		if (currentLight.compare("0") == 0) {
			light_->setText("Please select a light to change");
		}
		else {
			Http::Client *client = LightsControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"bri\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Brightness has been changed");

			//CHANGE DB ENTRY
			Light *x;
			x = session_->getLight("Hue Lamp " + currentLight);
			x->setBrightness(stoi(input));
			session_->updateLight(x);

			//test db changes
			x = session_->getLight("Hue Lamp " + currentLight);
			change_->setText("new Brightness: " + to_string(x->getBrightness()));
		}
	}
}


//changes the saturation
void LightsControlWidget::sat() {
	change_->setText("");
	//get user input and check if it is a positive integer
	std::string input = satEdit_->text().toUTF8();
	bool num = true;
	for (int i = 0; i < input.length(); i++) {
		if (!(isdigit(input[i]))) {
			num = false;
		}
	}

	//change saturation if input is a valid saturation input
	int value = atoi(input.c_str());
	if ((!num) || (value < 0) || (value > 254)) {
		change_->setText("Please input integer between 0 and 254 to change the saturation");
	}
	else {
		if (currentLight.compare("0") == 0) {
			light_->setText("Please select a light to change");
		}
		else {
			Http::Client *client = LightsControlWidget::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"sat\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightsControlWidget::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Saturation has been changed");

			// CHANGE DB ENTRY
			Light *x;
			x = session_->getLight("Hue Lamp " + currentLight);
			x->setSaturation(stoi(input));
			session_->updateLight(x);

			//test db changes
			x = session_->getLight("Hue Lamp " + currentLight);
			change_->setText("new Saturation: " + to_string(x->getSaturation()));
		}
	}
}