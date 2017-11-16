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


	new WText("BLAH ");
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


	//change name
	this->addWidget(new WText("Set New Name: "));
	nameEdit_ = new WLineEdit(this);												// user text input
	nameEdit_->setFocus();
	WPushButton *nameButton
		= new WPushButton("Change", this);										// submit button
	nameButton->setMargin(5, Left);
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
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	light_ = new WText(this);                           // displays which light is being changed
	this->addWidget(new WBreak());
	change_ = new WText(this);                          //displays the status of a light change
	this->addWidget(new WBreak());
	WPushButton *returnButton							//go back to bridge
		= new WPushButton("Return To Bridge", this);

	onButton->clicked().connect(this, &GroupsControlWidget::on);
	nameButton->clicked().connect(this, &GroupsControlWidget::name);
	offButton->clicked().connect(this, &GroupsControlWidget::off);
	oneButton->clicked().connect(this, &GroupsControlWidget::lightOne);
	twoButton->clicked().connect(this, &GroupsControlWidget::lightTwo);
	threeButton->clicked().connect(this, &GroupsControlWidget::lightThree);
	returnButton->clicked().connect(this, &GroupsControlWidget::returnBridge);
	briScaleSlider_->valueChanged().connect(this, &GroupsControlWidget::bright);
	satScaleSlider_->valueChanged().connect(this, &GroupsControlWidget::sat);
	hueScaleSlider_->valueChanged().connect(this, &GroupsControlWidget::hue);


	(boost::bind(&GroupsControlWidget::hue, this));
	(boost::bind(&GroupsControlWidget::name, this));
	(boost::bind(&GroupsControlWidget::bright, this));
	(boost::bind(&GroupsControlWidget::sat, this));
	(boost::bind(&GroupsControlWidget::on, this));
	(boost::bind(&GroupsControlWidget::off, this));
	(boost::bind(&GroupsControlWidget::handleHttpResponse, this));
	(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this));
	(boost::bind(&GroupsControlWidget::connect, this));
	(boost::bind(&GroupsControlWidget::lightOne, this));
	(boost::bind(&GroupsControlWidget::lightTwo, this));
	(boost::bind(&GroupsControlWidget::lightThree, this));
	(boost::bind(&GroupsControlWidget::returnBridge, this));
}

//creates a client
Http::Client * GroupsControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handle request (does nothing withthe response) - for changing the light state
void GroupsControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

//handles get lights request
void GroupsControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		Json::Object result;
		Json::parse(response.body(), result);

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

		hueScaleSlider_->setValue(stoi(hue));
		satScaleSlider_->setValue(stoi(sat));
		briScaleSlider_->setValue(stoi(bri));
	}
}


//selects light 1 to change
void GroupsControlWidget::lightOne() {
	currentLight = "1";
	light_->setText("You are changing Light 1");
	change_->setText("");
	Http::Client *client = GroupsControlWidget::connect();
	client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/lights/1")) {
		WApplication::instance()->deferRendering();
	}
}

//selects light 2 to change
void GroupsControlWidget::lightTwo() {
	currentLight = "2";
	light_->setText("You are changing Light 2");
	change_->setText("");
	Http::Client *client = GroupsControlWidget::connect();
	client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/lights/2")) {
		WApplication::instance()->deferRendering();
	}
}

//selects light 3 to change
void GroupsControlWidget::lightThree() {
	currentLight = "3";
	light_->setText("You are changing Light 3");
	change_->setText("");
	Http::Client *client = GroupsControlWidget::connect();
	client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponse, this, _1, _2));
	if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/lights/3")) {
		WApplication::instance()->deferRendering();
	}
}

//changes the name
void GroupsControlWidget::name() {
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		std::string input = nameEdit_->text().toUTF8();
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"name\" : \"" + input + "\"}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight, *msg);
		change_->setText("New Name: " + input);
	}
}


//turns light on
void GroupsControlWidget::on() {
	change_->setText("");
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : true}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight + "/state", *msg);
		change_->setText("Light: ON");
	}
}

//turns light off
void GroupsControlWidget::off() {
	change_->setText("");
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
	}
	else {
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"on\" : false}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight + "/state", *msg);
		change_->setText("Light: OFF");
	}
}

//changes the hue 
void GroupsControlWidget::hue() {
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
		change_->setText("");
	}
	else {
		int input = hueScaleSlider_->value();
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"hue\" : \"" + to_string(input) + "\"}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight + "/state", *msg);
		change_->setText("new Hue: " + to_string(input));
	}
}

//changes the brightness
void GroupsControlWidget::bright() {
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
		change_->setText("");
	}
	else {
		int input = briScaleSlider_->value();
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"bri\" : \"" + to_string(input) + "\"}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight + "/state", *msg);
		change_->setText("new Brightness: " + to_string(input));
	}
}

//changes the saturation
void GroupsControlWidget::sat() {
	if (currentLight.compare("0") == 0) {
		light_->setText("Please select a light to change");
		change_->setText("");
	}
	else {
		int input = satScaleSlider_->value();
		Http::Client *client = GroupsControlWidget::connect();
		Http::Message *msg = new Http::Message();
		msg->addBodyText("{\"sat\" : \"" + to_string(input) + "\"}");
		client->done().connect(boost::bind(&GroupsControlWidget::handleHttpResponseVOID, this, _1, _2));
		client->put("http://" + ip + ":" + port + "/api/" + userID + "/lights/" + currentLight + "/state", *msg);
		change_->setText("new Saturation: " + to_string(input));
	}
}

void GroupsControlWidget::returnBridge()
{
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}