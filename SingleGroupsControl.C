#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
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
	change_ = new WText(this);                          //displays the status of a light change
	this->addWidget(new WBreak());
	this->addWidget(new WBreak());
	
	WPushButton *deleteButton							//delete group
		= new WPushButton("Delete This Group", this);

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
	nameButton->clicked().connect(this, &SingleGroupsControlWidget::name);
	offButton->clicked().connect(this, &SingleGroupsControlWidget::off);
	returnButton->clicked().connect(this, &SingleGroupsControlWidget::returnBridge);
	deleteButton->clicked().connect(this, &SingleGroupsControlWidget::deleteGroup);
	briScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::bright);
	satScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::sat);
	hueScaleSlider_->valueChanged().connect(this, &SingleGroupsControlWidget::hue);


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
		string lights = subString.substr(0, endPos);
		boost::erase_all(lights, "\"");
		groupInfoEdit_->setText("Group Name: " + name);
		groupLightsEdit_->setText("Lights in your Group: " + lights);
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
		WApplication::instance()->setInternalPath("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port, true);
	}
}

void SingleGroupsControlWidget::addLights() {
}

void SingleGroupsControlWidget::removeLights() {
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


void SingleGroupsControlWidget::returnBridge(){
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
