// main.cpp : Defines the entry point for the console application. Includes the Wt application 

#include <stdio.h>
#include <iostream>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WServer>
#include <Wt/Http/Message>
#include <Wt/Http/Client>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include "MyResource.h"
#include "log.h"
#include "Light.h"

using namespace Wt;
using namespace std;


class LightApplication : public WApplication
{
public:
	LightApplication(const WEnvironment& env);

private:
	WLineEdit *hueEdit_;
	WLineEdit *satEdit_;
	WLineEdit *briEdit_;
	WText *change_;
	WText *light_;
	string currentLight = "0";

	void on();
	void off();
	void hue();
	void bright();
	void sat();
	void handleHttpResponse(boost::system::error_code err, const Http::Message& response);
	Http::Client * connect();
	void lightOne();
	void lightTwo();
	void lightThree();
};


LightApplication::LightApplication(const WEnvironment& env)
	: WApplication(env)
{
	//get info about 

	/*
	for (int i = 1; i <= 3; i++){
	Http::Client *client = LightApplication::connect();
	client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
	if (client->get("http://localhost:8000/api/newdeveloper/lights/" + to_string(i))) {
	WApplication::instance()->deferRendering();
	}
	}
	*/

	Http::Client *client = LightApplication::connect();
	client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
	if (client->get("http://localhost:8000/api/newdeveloper/lights/3")) {
		WApplication::instance()->deferRendering();
	}

	setTitle("3307 HUE");

	//select the light to be changed
	root()->addWidget(new WText("Select the light to be changed: "));
	WPushButton *oneButton
		= new WPushButton("Light 1", root());										// 1st light button
	oneButton->setMargin(5, Left);
	WPushButton *twoButton
		= new WPushButton("Light 2", root());										// 2nd light button
	twoButton->setMargin(5, Left);
	WPushButton *threeButton
		= new WPushButton("Light 3", root());										// 3rd light button
	threeButton->setMargin(5, Left);
	root()->addWidget(new WBreak());
	root()->addWidget(new WBreak());

	//turn on
	root()->addWidget(new WText("Light on/off: "));
	WPushButton *onButton
		= new WPushButton("ON", root());											// ON button
	onButton->setMargin(5, Left);													// add 5 pixels margin

																					//turn off
	WPushButton *offButton
		= new WPushButton("OFF", root());											// OFF button
	offButton->setMargin(5, Left);													// add 5 pixels margin
	root()->addWidget(new WBreak());												// insert a line break
	root()->addWidget(new WBreak());

	//change hue
	root()->addWidget(new WText("Hue: "));
	hueEdit_ = new WLineEdit(root());												// user text input
	hueEdit_->setFocus();															// give focus
	WPushButton *hueButton
		= new WPushButton("Change", root());										// submit button
	hueButton->setMargin(5, Left);													// add 5 pixels margin
	root()->addWidget(new WBreak());												// insert a line break
	root()->addWidget(new WBreak());

	//change brightness
	root()->addWidget(new WText("Brightness: "));
	briEdit_ = new WLineEdit(root());												// user text input
	briEdit_->setFocus();															// give focus
	WPushButton *brightButton
		= new WPushButton("Change", root());										// submit button
	brightButton->setMargin(5, Left);												// add 5 pixels margin
	root()->addWidget(new WBreak());												// insert a line break
	root()->addWidget(new WBreak());

	//change saturation
	root()->addWidget(new WText("Saturation: "));
	satEdit_ = new WLineEdit(root());												// user text input
	satEdit_->setFocus();															// give focus
	WPushButton *satButton
		= new WPushButton("Change", root());										// submit button
	satButton->setMargin(5, Left);													// add 5 pixels margin
	root()->addWidget(new WBreak());												// insert a line break
	root()->addWidget(new WBreak());


	light_ = new WText(root());														// displays which light is being changed
	root()->addWidget(new WBreak());
	change_ = new WText(root());													//displays the status of a light change

	hueButton->clicked().connect(this, &LightApplication::hue);
	brightButton->clicked().connect(this, &LightApplication::bright);
	satButton->clicked().connect(this, &LightApplication::sat);
	onButton->clicked().connect(this, &LightApplication::on);
	offButton->clicked().connect(this, &LightApplication::off);
	oneButton->clicked().connect(this, &LightApplication::lightOne);
	twoButton->clicked().connect(this, &LightApplication::lightTwo);
	threeButton->clicked().connect(this, &LightApplication::lightThree);

	(boost::bind(&LightApplication::hue, this));
	(boost::bind(&LightApplication::bright, this));
	(boost::bind(&LightApplication::sat, this));
	(boost::bind(&LightApplication::on, this));
	(boost::bind(&LightApplication::off, this));
	(boost::bind(&LightApplication::handleHttpResponse, this));
	(boost::bind(&LightApplication::connect, this));
	(boost::bind(&LightApplication::lightOne, this));
	(boost::bind(&LightApplication::lightTwo, this));
	(boost::bind(&LightApplication::lightThree, this));
}

//creates a client
Http::Client * LightApplication::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handles http request
void LightApplication::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
}

//selects light 1 to change
void LightApplication::lightOne() {
	currentLight = "1";
	light_->setText("You are changing Light 1");
	change_->setText("");
}

//selects light 2 to change
void LightApplication::lightTwo() {
	currentLight = "2";
	light_->setText("You are changing Light 2");
	change_->setText("");

}

//selects light 3 to change
void LightApplication::lightThree() {
	currentLight = "3";
	light_->setText("You are changing Light 3");
	change_->setText("");
}

//changes the hue 
void LightApplication::hue() {
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
			Http::Client *client = LightApplication::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"hue\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Hue has been changed");
		}
	}
}


//turns light on
void LightApplication::on() {
	change_->setText("");
	Http::Client *client = LightApplication::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : true}");
	client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
		change_->setText("Light has been turned on");
	}
}

//turns light off
void LightApplication::off() {
	change_->setText("");
	Http::Client *client = LightApplication::connect();
	Http::Message *msg = new Http::Message();
	msg->addBodyText("{\"on\" : false}");
	client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
		change_->setText("Light has been turned off");
	}
}

//changes the brightness
void LightApplication::bright() {
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
			Http::Client *client = LightApplication::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"bri\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Brightness has been changed");
		}
	}
}


//changes the saturation
void LightApplication::sat() {
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
			Http::Client *client = LightApplication::connect();
			Http::Message *msg = new Http::Message();
			msg->addBodyText("{\"sat\" : \"" + input + "\"}");
			client->done().connect(boost::bind(&LightApplication::handleHttpResponse, this, _1, _2));
			client->put("http://localhost:8000/api/newdeveloper/lights/" + currentLight + "/state", *msg);
			change_->setText("Saturation has been changed");
		}
	}
}


WApplication *createApplication(const WEnvironment& env)
{
	return new LightApplication(env);
}

int main(int argc, char **argv)
{
	//from the helper link on OWL
	WLogger logger;
	configLogger(logger);
	try {
		WServer server(argv[0], "");
		try {
			server.setServerConfiguration(argc, argv);

			//from the helper link on OWL
			Wt::WServer server(argv[0]);
			server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
			server.addResource(new MyResource, "/people");
			server.addEntryPoint(Wt::Application, createApplication);

			info(logger, "Starting resource server.");
			if (server.start()) {
				WServer::waitForShutdown();
				server.stop();
			}
			else {
				fatal(logger, "Fatal error starting resource server.");
				return 1;
			}
			return 0;
		}
		catch (std::exception& e) {
			fatal(logger, "Fatal error starting resource server.", e.what());
			return 1;
		}
	}
	catch (WServer::Exception& e) {
		fatal(logger, "Fatal error creating WServer.", e.what());
		return 1;
	}
	catch (exception& e) {
		fatal(logger, "Fatal error occurred.", e.what());
		return 1;
	}
}




