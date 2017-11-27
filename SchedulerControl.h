#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


#ifndef SCHEDULERCONTROL_H_
#define SCHEDULERCONTROL_H_

struct dateTime {
  std::string name; 
  char light; 
  int on; 
  int hue; 
  int bri;
  int sat;
  int transition; 
  int year;
  int month;
  int day; 
  std::string hour; 
  std::string min; 
  std::string sec; 
  std::string merDes; 

};

class Session;

class SchedulerControlWidget: public Wt::WContainerWidget
{
public:
  SchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
  void update();

private:
	Session *session_;
	Wt::WLineEdit *nameEdit_;
	Wt::WLineEdit *hueEdit_;
	Wt::WSlider *satScaleSlider_;
	Wt::WSlider *briScaleSlider_;
	Wt::WSlider *hueScaleSlider_;
	Wt::WSlider *transitionScaleSlider_;
	Wt::WCalendar *calendar_;
	Wt::WComboBox *hourInput_ ;
	Wt::WComboBox *minInput_ ;
	Wt::WComboBox *secInput_ ;
	Wt::WComboBox *amSelector_; 
	Wt::WText *oneLight_;
	Wt::WText *twoLight_;
	Wt::WText *threeLight_;
	Wt::WText *change_;
	Wt::WText *light_;
	Wt::WText *dateSelect_; 
	std::string currentLight = "0";
	std::string ip = "";
	std::string userID = "";
	std::string port = "";
	dateTime inputData; 
	void on();
	void off();
	void hue();
	void bright();
	void sat();
	void name();
	void transition();
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseName(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void lightOne();
	void lightTwo();
	void lightThree();
	void returnBridge();
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
