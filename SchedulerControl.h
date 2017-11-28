// SchedulesControl.h : Defines the SchedulesControl class that manages the creation and listing of Schedules
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef SCHEDULERCONTROL_H_
#define SCHEDULERCONTROL_H_

class Session;

class SchedulerControlWidget: public Wt::WContainerWidget
{
public:
	SchedulerControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	void update();

private:
	int numOfSchedules; 
	Session *session_;
	std::string ip = "";
	std::string userID = "";
	std::string port = "";
	bool one = false;
	bool two = false;
	bool three = false;
	Wt::WLineEdit *nameEdit_;
	Wt::WPushButton *createButton;
	Wt::WText *status_;
	Wt::WText *Schedules_;
	Wt::Http::Client * connect();
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	
	void createSchedule();
	void returnBridge();
};

#endif
