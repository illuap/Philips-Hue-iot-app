#include <Wt/WContainerWidget>

#include "Session.h"
#include "LightsControl.h"
#include "BridgeControl.h"
#include "GroupsControl.h"
#include "SingleGroupsControl.h"
#include "BridgeEditControl.h"
#include "SchedulerControl.h"
#include "SingleSchedulerControl.h"
#include "GroupsSchedulerControl.h"



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
class SchedulerControlWidget;
class SingleSchedulerControllWidget;
class GroupsSchedulerControlWidget;


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
  SchedulerControlWidget * the_Schedulers; 
  SingleSchedulerControlWidget * the_SingleSchedulers; 
  GroupsSchedulerControlWidget * the_GroupSchedulers; 

  Wt::WAnchor *backToGameAnchor_;

  Session session_;

  void onAuthEvent();
  void showLights();
  void showBridge();
  void showGroups();
  void showSingleGroups();
  void showBridgeEdit();
  void showSchedulers(); 
  void showSingleSchedulers(); 
  void showGroupScheduler(); 

};

#endif //HUEAPP_H_
