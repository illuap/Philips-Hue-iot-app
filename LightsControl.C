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

    // Light *x = new Light(name, type, stoi(bri), stoi(hue), stoi(sat), lightOn, 0);

    /*
    
    INSERT X IN DATABASE HERE
    
    
    
    */
  }
}
Http::Client * LightsControlWidget::connect() {
  Http::Client *client = new Http::Client(this);
  client->setTimeout(15);
  client->setMaximumResponseSize(10 * 1024);
}
