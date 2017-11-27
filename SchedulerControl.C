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
#include <Wt/WCalendar>
#include <Wt/WTimeEdit>
#include <Wt/WTime>
#include <Wt/WDate>
#include <Wt/WComboBox>
#include <Wt/Http/Client>
#include <string>
#include "SchedulerControl.h"
#include "Session.h"


using namespace Wt;
using namespace std; 

SchedulerControlWidget::SchedulerControlWidget(Session *session, WContainerWidget *parent):
  WContainerWidget(parent),
  session_(session)
{
  setContentAlignment(AlignCenter);
  setStyleClass("highscores");
}

void SchedulerControlWidget::update()
{
  clear();
  inputData.light = '0';
  inputData.hour = "01"; 
  inputData.min = "00";
  inputData.sec = "00"; 
 
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

  //get group info to display 
  Http::Client *client = SchedulerControlWidget::connect();
  client->done().connect(boost::bind(&SchedulerControlWidget::handleHttpResponseName, this, _1, _2));
  if (client->get("http://" + ip + ":" + port + "/api/" + userID + "/lights/")) {
	  WApplication::instance()->deferRendering();
  }

  this->addWidget(new WText("Select the light to be changed: "));
  this->addWidget(new WBreak());
  WPushButton *oneButton  = new WPushButton("Light 1", this);                   // 1st light button
  oneButton->setMargin(10, Left);
  oneLight_ = new WText(this);
  this->addWidget(new WBreak());
  WPushButton *twoButton = new WPushButton("Light 2", this);					//2nd light button
  twoButton->setMargin(10, Left);
  twoLight_ = new WText(this);
  this->addWidget(new WBreak());
  WPushButton *threeButton = new WPushButton("Light 3", this);                   // 3rd light button
  threeButton->setMargin(10, Left);
  threeLight_ = new WText(this);
  this->addWidget(new WBreak());
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
  light_ = new WText(this);                           // displays which light is being changed
  this->addWidget(new WBreak());
  change_ = new WText(this);                          //displays the status of a light change
  this->addWidget(new WBreak());
	
  this->addWidget(new WText("Scheduler")); 

  this->addWidget(new WBreak());
  hourInput_ = new WComboBox(this);
   for (int i = 1; i<13; i++){
      if (i<10){
        hourInput_->addItem("0"+to_string(i));
      }
      else{
        hourInput_->addItem(to_string(i));
      }
      
  }
  this->addWidget(new WText(":")); 
  minInput_ = new WComboBox(this);
  for (int i = 0; i<60; i++){
      if (i<10){
        minInput_->addItem("0"+to_string(i));
      }
      else{
        minInput_->addItem(to_string(i));
      }
  }
  this->addWidget(new WText(":")); 
  secInput_ = new WComboBox(this);
  for (int i = 0; i<60; i++){
      if (i<10){
        secInput_->addItem("0"+to_string(i));
      }
      else{
        secInput_->addItem(to_string(i));
      }
  }
  amSelector_ = new WComboBox(this);
  amSelector_->addItem("AM");
  amSelector_->addItem("PM");

  this->addWidget(new WBreak());
  dateSelect_ = new WText(this);
  dateSelect_->setText("Selected Date:          ");
  this->addWidget(new WBreak());
  calendar_ = new WCalendar(this);
  calendar_->setSingleClickSelect(true);
  this->addWidget(new WBreak());


  WPushButton *scheduleButton = new WPushButton("Create Schedule", this);                      // ON button
  onButton->setMargin(5, Left);


  WPushButton *groupButton						
	  = new WPushButton("Go to My Groups", this);
  groupButton->setLink("/?_=/group?user=" + userID + "%26ip=" + ip + "%26port=" + port);
  groupButton->setMargin(10, Left);


  WPushButton *returnButton							//go back to bridge
	  = new WPushButton("Return To Bridge", this);

  onButton->clicked().connect(this, &SchedulerControlWidget::on);
  nameButton->clicked().connect(this, &SchedulerControlWidget::name);
  offButton->clicked().connect(this, &SchedulerControlWidget::off);
  oneButton->clicked().connect(this, &SchedulerControlWidget::lightOne);
  twoButton->clicked().connect(this, &SchedulerControlWidget::lightTwo);
  threeButton->clicked().connect(this, &SchedulerControlWidget::lightThree);
  returnButton->clicked().connect(this, &SchedulerControlWidget::returnBridge);
  briScaleSlider_->valueChanged().connect(this, &SchedulerControlWidget::bright);
  satScaleSlider_->valueChanged().connect(this, &SchedulerControlWidget::sat);
  hueScaleSlider_->valueChanged().connect(this, &SchedulerControlWidget::hue);
  transitionScaleSlider_->valueChanged().connect(this, &SchedulerControlWidget::transition);
  calendar_->clicked().connect(this, &SchedulerControlWidget::changeDate);
  hourInput_->changed().connect(this, &SchedulerControlWidget::changeHour);
  minInput_->changed().connect(this, &SchedulerControlWidget::changeMin);
  secInput_->changed().connect(this, &SchedulerControlWidget::changeSec);

  scheduleButton->clicked().connect(this, &SchedulerControlWidget::createSchedule);




  (boost::bind(&SchedulerControlWidget::transition, this));
  (boost::bind(&SchedulerControlWidget::hue, this));
  (boost::bind(&SchedulerControlWidget::name, this));
  (boost::bind(&SchedulerControlWidget::bright, this));
  (boost::bind(&SchedulerControlWidget::sat, this));
  (boost::bind(&SchedulerControlWidget::on, this));
  (boost::bind(&SchedulerControlWidget::off, this));
  (boost::bind(&SchedulerControlWidget::handleHttpResponse, this));
  (boost::bind(&SchedulerControlWidget::handleHttpResponseVOID, this));
  (boost::bind(&SchedulerControlWidget::connect, this));
  (boost::bind(&SchedulerControlWidget::lightOne, this));
  (boost::bind(&SchedulerControlWidget::lightTwo, this));
  (boost::bind(&SchedulerControlWidget::lightThree, this));
  (boost::bind(&SchedulerControlWidget::returnBridge, this));
  (boost::bind(&SchedulerControlWidget::changeDate, this));
  (boost::bind(&SchedulerControlWidget::changeHour, this));
  (boost::bind(&SchedulerControlWidget::changeMin, this));
  (boost::bind(&SchedulerControlWidget::changeSec, this));
  (boost::bind(&SchedulerControlWidget::createSchedule, this));




}

//creates a client
Http::Client * SchedulerControlWidget::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handle request (does nothing withthe response) - for changing the light state
void SchedulerControlWidget::handleHttpResponseName(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		Json::Object result;
		Json::parse(response.body(), result);

		size_t pos = response.body().find("name");
		string subString = response.body().substr(pos + 6);
		size_t endPos = subString.find(",");
		string name = subString.substr(0, endPos);
		boost::erase_all(name, "\"");
		oneLight_->setText("	(" + name + ")");
	
		pos = subString.find("name");
		subString = subString.substr(pos + 6);
		endPos = subString.find(",");
		name = subString.substr(0, endPos);
		boost::erase_all(name, "\"");
		twoLight_->setText("	(" + name + ")");

		pos = subString.find("name");
		subString = subString.substr(pos + 6);
		endPos = subString.find(",");
		name = subString.substr(0, endPos);
		boost::erase_all(name, "\"");
		threeLight_->setText("	(" + name + ")");
	}

}

//handle request (does nothing withthe response) - for changing the light state
void SchedulerControlWidget::handleHttpResponseVOID(boost::system::error_code err, const Http::Message& response) {
}

//handles get lights request
void SchedulerControlWidget::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
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

void SchedulerControlWidget::changeDate() {
	std::set<Wt::WDate> selection = (calendar_->selection());
	WDate date = (*selection.begin());
	
  inputData.year = date.year(); 
  inputData.month = date.month(); 
  inputData.day = date.day(); 
	std::string selectYear = to_string(inputData.year);
	std::string selectMonth = to_string(inputData.month); 
	std::string selectDay = to_string(inputData.day); 
  std::string output = "Selected Date:" + selectYear; 
  //Formats output string
  if (inputData.month<10){
   output +="/0"+selectMonth;
  }
  else{
    output += "/"+selectMonth;
  }
  if (inputData.day<10){
   output +="/0"+selectDay;
  }
  else{
    output +="/"+selectDay;
  }
  dateSelect_->setText(output); 
	

}
//selects light 1 to change
void SchedulerControlWidget::lightOne() {
	inputData.light = '1';
	light_->setText("You are changing Light 1     " + oneLight_->text());
	change_->setText("");
	
}

//selects light 2 to change
void SchedulerControlWidget::lightTwo() {
	inputData.light = '2';
	light_->setText("You are changing Light 2     " + twoLight_->text());
	change_->setText("");
	
}

//selects light 3 to change
void SchedulerControlWidget::lightThree() {
	inputData.light = '3';
	light_->setText("You are changing Light 3     " + threeLight_->text());
	change_->setText("");
}

//changes the name
void SchedulerControlWidget::name() {
  std::string input = nameEdit_->text().toUTF8();
  inputData.name = input; 
}


//turns light on
void SchedulerControlWidget::on() {
  inputData.on = 1; 
  change_->setText("Light: ON");
}

//turns light off
void SchedulerControlWidget::off() {
  inputData.on = 2;
  change_->setText("Light: OFF");
}

//changes the hue 
void SchedulerControlWidget::hue() {
  int input = hueScaleSlider_->value();

  inputData.hue = input;
  change_->setText("new Hue: " + to_string(input));
}

//changes the brightness
void SchedulerControlWidget::bright() {
  int input = briScaleSlider_->value();
  inputData.bri = input;
  change_->setText("new Brightness: " + to_string(input));
}

//changes the saturation
void SchedulerControlWidget::sat() {
  int input = satScaleSlider_->value();
  inputData.sat = input;
  change_->setText("new Saturation: " + to_string(input));
}

//changes the transition time
void SchedulerControlWidget::transition() {
  int input = transitionScaleSlider_->value();
  inputData.transition = input;
  change_->setText("new Transition Time: " + to_string(input * 100) + "ms");

}

void SchedulerControlWidget::changeHour(){
  inputData.hour = hourInput_->currentText().toUTF8();
}

void SchedulerControlWidget::changeMin(){
  inputData.min = minInput_->currentText().toUTF8();
}

void SchedulerControlWidget::changeSec(){
  inputData.sec = secInput_->currentText().toUTF8();
}

void SchedulerControlWidget::createSchedule(){
  if ( inputData.light == NULL) {
   light_->setText("Please select a light to change");
   change_->setText("");
  } else {
   Http::Client *client = SchedulerControlWidget::connect();
   Http::Message *msg = new Http::Message();
   change_->setText(createPostMessage());
   msg->addBodyText(createPostMessage());
   client->done().connect(boost::bind(&SchedulerControlWidget::handleHttpResponseVOID, this, _1, _2));
   client->post("http://" + ip + ":" + port + "/api/" + userID + "/schedules/", *msg);
   inputData = {};
   // client->post("http://" + ip + ":" + port + "/api/newdeveloper/debugger/schedules/", *msg);

   // change_->setText("Schedule Created");
  }
}




std::string SchedulerControlWidget::createPostMessage(){
  std::string postMessage = "{"; 
  postMessage += "\"command\": {";
  // postMessage += "\"address\": \"/api/newdeveloper/debugger/lights/";
  // postMessage += inputData.light + "/state\" ,";
  postMessage += "\"address\": \"/api/" + userID + "/lights/" + inputData.light + "/state\" ,";
  postMessage += "\"method\": \"PUT\","; 
  postMessage += "\"body\":" + createBodyMessage(); 
  postMessage += "},";
  postMessage += createDateTime(); 
  postMessage += "}"; 
  return postMessage; 
}

//Creates the light changes in a string to input into body of post
std::string SchedulerControlWidget::createBodyMessage(){
  std::string bodyText = "{"; 

  if (inputData.on != NULL){
    if (inputData.on == 1){
       bodyText += "\"on\": true,";
    }
    else if (inputData.on == 2){
       bodyText += "\"on\": false,";
    }
  }
  if (inputData.hue != NULL){
      bodyText += "\"hue\":"+ to_string(inputData.hue)+",";
  }
  if (inputData.bri != NULL){
      bodyText += "\"bri\":"+ to_string(inputData.bri)+",";
  }
  if (inputData.sat != NULL){
      bodyText += "\"sat\":"+ to_string(inputData.sat)+",";
  }
  if (inputData.transition != NULL){
      bodyText += "\"transitiontime\":"+ to_string(inputData.transition)+",";
  }
  bodyText.pop_back();
  bodyText += "}"; 
  return bodyText; 

}

std::string SchedulerControlWidget::createDateTime(){
  std::string dateTimeMessage = "\"time\":"; 
  dateTimeMessage += "\""+ to_string(inputData.year)+"-"+to_string(inputData.month)+"-"+to_string(inputData.day);
  dateTimeMessage += "T";
  if ((amSelector_->currentText().toUTF8()).compare("PM") == 0 ){
    int currentHour = stoi(inputData.hour);
    if (currentHour<=12){
      currentHour += 12; 
      inputData.hour = to_string(currentHour); 
    }
    
  }
  dateTimeMessage += inputData.hour + ":"+inputData.min + ":" + inputData.sec + "\"";
  return dateTimeMessage; 

}
void SchedulerControlWidget::returnBridge()
{
	clear();
	WApplication::instance()->setInternalPath("/Bridge", true);
}
