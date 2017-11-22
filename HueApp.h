// This may look like C code, but it's really -*- C++ -*-
/* 
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>

#include "Session.h"
#include "LightsControl.h"
#include "BridgeControl.h"
#include "GroupsControl.h"
#include "SingleGroupsControl.h"
#include "BridgeEditControl.h"

#ifndef HUEAPP_H_
#define HUEAPP_H_




namespace Wt {
  class WStackedWidget;
  class WAnchor;
}

class Session;
class LightsControlWidget; 
class BridgeControlWidget;
class GroupsControlWidget;
class SingleGroupsControlWidget;
class BridgeEditControlWidget;

class HueApp : public Wt::WContainerWidget
{
public:
  HueApp(Wt::WContainerWidget *parent = 0);

  void handleInternalPath(const std::string &internalPath);

private:
  Wt::WStackedWidget *mainStack_;
  LightsControlWidget *the_Lights;
  BridgeControlWidget *the_Bridge;
  GroupsControlWidget *the_Groups;
  SingleGroupsControlWidget *the_SingleGroups;
  BridgeEditControlWidget *the_BridgeEdit;
  // Wt::WContainerWidget *links_;
  Wt::WAnchor *backToGameAnchor_;


  Session session_;

  void onAuthEvent();
  void showLights();
  void showBridge();
  void showGroups();
  void showSingleGroups();
  void showBridgeEdit();

};

#endif //HUEAPP_H_
