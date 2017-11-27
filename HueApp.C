/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WAnchor>
#include <Wt/WText>
#include <Wt/WStackedWidget>
#include <Wt/WVBoxLayout>
#include <Wt/WHBoxLayout>
#include <Wt/WApplication>
#include <Wt/Auth/AuthWidget>

#include "HueApp.h"
#include "AuthWidget.h"

using namespace Wt;

HueApp::HueApp(WContainerWidget *parent):
  WContainerWidget(parent),
  the_Lights(0),
  the_Bridge(0),
  the_Groups(0),
  the_SingleGroups(0),
  the_BridgeEdit(0),
  the_Schedulers(0)

{
  session_.login().changed().connect(this, &HueApp::onAuthEvent);

  Auth::AuthModel *authModel = new Auth::AuthModel(Session::auth(),
               session_.users(), this);
  authModel->addPasswordAuth(&Session::passwordAuth());
  authModel->addOAuth(Session::oAuth());

  AuthWidget *authWidget = new AuthWidget(session_);
  authWidget->setModel(authModel);
  authWidget->setRegistrationEnabled(true);

  WText *title = new WText("<h1>NWPD Light App</h1>");
  addWidget(title);

  addWidget(authWidget);

  mainStack_ = new WStackedWidget();
  mainStack_->setStyleClass("gamestack");
  addWidget(mainStack_);

  // links_ = new WContainerWidget();
  // links_->setStyleClass("links");
  // links_->hide();
  // addWidget(links_);

/*
  backToGameAnchor_ = new WAnchor("/lights", "Lights1", links_);
  backToGameAnchor_->setLink(WLink(WLink::InternalPath, "/lights"));
*/
 

  WApplication::instance()->internalPathChanged()
    .connect(this, &HueApp::handleInternalPath);

  authWidget->processEnvironment();
  
}

void HueApp::onAuthEvent()
{
  if (session_.login().loggedIn()) {  
    // links_->show();
    handleInternalPath(WApplication::instance()->internalPath());
  } else {
    mainStack_->clear();
    the_Lights = 0;
    the_Bridge = 0;
	the_Groups = 0;
	the_SingleGroups = 0;
	the_BridgeEdit = 0;
     the_Schedulers = 0;

    // links_->hide();
  }
}

void HueApp::handleInternalPath(const std::string &internalPath)
{
  if (session_.login().loggedIn()) {
	  if (internalPath.find("/light") == 0)
		  showLights();
	  else if (internalPath.find("/bridge") == 0)
		  showBridge();
	else if (internalPath.find("/group") == 0)
		  showGroups();
	else if (internalPath.find("/singlegroup") == 0)
		  showSingleGroups();
	else if (internalPath.find("/editbridge") == 0)
		  showBridgeEdit();
  	else if (internalPath.find("/scheduler") == 0)
    		 showSchedulers();
    else
      WApplication::instance()->setInternalPath("/bridge",  true);
  }
}

void HueApp::showLights()
{
  if (!the_Lights)
    the_Lights = new LightsControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Lights);
  the_Lights->update();

  // hueLights_->addStyleClass("selected-link");
}

void HueApp::showBridge(){
  if (!the_Bridge)
    the_Bridge = new BridgeControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Bridge);
  the_Bridge->update();

  // hueLights_->removeStyleClass("selected-link");
}

void HueApp::showGroups()
{
	if (!the_Groups)
		the_Groups = new GroupsControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_Groups);
	the_Groups->update();

	// hueLights_->addStyleClass("selected-link");
}

void HueApp::showSingleGroups()
{
	if (!the_SingleGroups)
		the_SingleGroups = new SingleGroupsControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_SingleGroups);
	the_SingleGroups->update();

	// hueLights_->addStyleClass("selected-link");
}

void HueApp::showBridgeEdit()
{
	if (!the_BridgeEdit)
		the_BridgeEdit = new BridgeEditControlWidget(&session_, mainStack_);

	mainStack_->setCurrentWidget(the_BridgeEdit);
	the_BridgeEdit->update();

	// hueLights_->addStyleClass("selected-link");
}
void HueApp::showSchedulers(){
  if (!the_Schedulers)
    the_Schedulers = new SchedulerControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Schedulers);
  the_Schedulers->update();

  // hueLights_->addStyleClass("selected-link");

}