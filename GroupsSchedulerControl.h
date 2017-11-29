/** @file groupsScheduleControl.h
*  @brief Application for creating and listing schedules of Groups
*  @author Weija Zhou
*  @date Nov 28, 2017
*/

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef GROUPSSCHEDULERCONTROL_H_
#define GROUPSSCHEDULERCONTROL_H_

class Session;

class GroupsSchedulerControlWidget: public Wt::WContainerWidget
{
public:
  GroupsSchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
  void update();

private:
  Session *session_;
  std::string ip = "";
  std::string userID = "";
  std::string port = "";
  std::string groupID = "";
  std::string lights;
  bool deleteConfirm;
  Wt::Http::Client * connect();
  Wt::WText *groupInfoEdit_;
  Wt::WText *groupLightsEdit_;
  Wt::WCalendar *calendar_;
  Wt::WText *dateSelect_; 
  Wt::WComboBox *hourInput_ ;
  Wt::WComboBox *minInput_ ;
  Wt::WComboBox *secInput_ ;
  Wt::WComboBox *amSelector_; 
  Wt::WLineEdit *hueEdit_;
  Wt::WSlider *satScaleSlider_;
  Wt::WSlider *briScaleSlider_;
  Wt::WSlider *hueScaleSlider_;
  Wt::WSlider *transitionScaleSlider_;
  Wt::WText *change_;
  Wt::WPushButton *scheduleButton; 


  int stateOn;
  int stateHue; 
  int stateBri;
  int stateSat; 
  int stateTrans; 
  int Datayear;
  int Datamonth;
  int Dataday; 
  std::string Datahour; 
  std::string Datamin; 
  std::string Datasec; 
  std::string DatamerDes; 

  void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
  void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);
  void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
  void on();
  void off();
  void hue();
  void bright();
  void sat();
  static void music(Wt::WSound *sound);
  void addLights();
  void removeLights();
  void deleteGroup();
  void returnBridge();
  void transition();
  void changeDate(); 
  void createSchedule(); 
  void changeHour(); 
  void changeMin(); 
  void changeSec(); 
  std::string createBodyMessage();
  std::string createPostMessage();
  std::string createDateTime();

};

#endif
