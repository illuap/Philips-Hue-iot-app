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

#include "HangmanGame.h"

using namespace Wt;

HangmanGame::HangmanGame(WContainerWidget *parent):
  WContainerWidget(parent),
  the_Lights(0),
  the_Bridge(0)
{
  session_.login().changed().connect(this, &HangmanGame::onAuthEvent);

  Auth::AuthModel *authModel = new Auth::AuthModel(Session::auth(),
               session_.users(), this);
  authModel->addPasswordAuth(&Session::passwordAuth());
  authModel->addOAuth(Session::oAuth());

  Auth::AuthWidget *authWidget = new Auth::AuthWidget(session_.login());
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
    .connect(this, &HangmanGame::handleInternalPath);

  authWidget->processEnvironment();
  
}

void HangmanGame::onAuthEvent()
{
  if (session_.login().loggedIn()) {  
    // links_->show();
    handleInternalPath(WApplication::instance()->internalPath());
  } else {
    mainStack_->clear();
    the_Lights = 0;
    the_Bridge = 0;
    // links_->hide();
  }
}

void HangmanGame::handleInternalPath(const std::string &internalPath)
{
  if (session_.login().loggedIn()) {
    if (internalPath.find("/light") == 0)
      showLights();
    else if(internalPath == "/bridge")
      showBridge();
    else
      WApplication::instance()->setInternalPath("/bridge",  true);
  }
}

void HangmanGame::showLights()
{
  if (!the_Lights)
    the_Lights = new LightsControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Lights);
  the_Lights->update();

  // hueLights_->addStyleClass("selected-link");
}

void HangmanGame::showBridge(){
  if (!the_Bridge)
    the_Bridge = new BridgeControlWidget(&session_, mainStack_);

  mainStack_->setCurrentWidget(the_Bridge);
  the_Bridge->update();

  // hueLights_->removeStyleClass("selected-link");
}