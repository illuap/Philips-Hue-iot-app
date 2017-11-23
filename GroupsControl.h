// GroupsControl.h : Defines the GroupsControl class that manages the creation and listing of groups
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef GROUPCONTROL_H_
#define GROUPCONTROL_H_

class Session;

class GroupsControlWidget: public Wt::WContainerWidget
{
public:
	GroupsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	void update();

private:
	Session *session_;
	std::string ip = "";
	std::string userID = "";
	std::string port = "";
	bool one = false;
	bool two = false;
	bool three = false;
	Wt::WLineEdit *nameEdit_;
	Wt::WText *light1_;
	Wt::WText *light2_;
	Wt::WText *light3_;
	Wt::WText *status_;
	Wt::WText *groups_;
	Wt::Http::Client * connect();
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	void lightOne();
	void lightTwo();
	void lightThree();
	void createGroup();
	void returnBridge();
};

#endif
