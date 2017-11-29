#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


#ifndef SINGLESCHEDULERCONTROL_H_
#define SINGLESCHEDULERCONTROL_H_


class Session;

class SingleSchedulerControlWidget: public Wt::WContainerWidget
{
public:
  SingleSchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
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
	Wt::WText *scheduleInfoEdit_;
	Wt::WText *scheduleTimeEdit_;
	Wt::WPushButton *deleteButton;
	Wt::WPushButton *scheduleButton; 
	std::string currentLight = "0";
	std::string ip = "";
	std::string userID = "";
	std::string port = "";
	std::string scheduleID = "";
	std::string nameID = "";
	std::string name; 
  	char Datalight; 
  	int Dataon; 
  	int Datahue; 
  	int Databri;
  	int Datasat;
  	int Datatransition; 
  	int Datayear;
  	int Datamonth;
  	int Dataday; 
 	std::string Datahour; 
  	std::string Datamin; 
  	std::string Datasec; 
  	std::string DatamerDes; 
	bool deleteConfirm; 
	void on();
	void off();
	void hue();
	void bright();
	void sat();
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
	void deleteSchedule();
	std::string createBodyMessage();
	std::string createPostMessage();
	std::string createDateTime();


};

#endif
