// LightsControl.h : Defines the LightsControl class that manages the states and changes of states in lights
// States include name, hue, brightness, transition time, saturation, on/off
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef LIGHTCONTROL_H_
#define LIGHTCONTROL_H_

class Session;

class LightsControlWidget: public Wt::WContainerWidget
{
public:
  LightsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
  void update();

private:
	Session *session_;
	std::string currentLight = "0";
	std::string ip = "";
	std::string userID = "";
	std::string port = "";
	Wt::WLineEdit *nameEdit_;
	Wt::WLineEdit *hueEdit_;
	Wt::WSlider *satScaleSlider_;
	Wt::WSlider *briScaleSlider_;
	Wt::WSlider *hueScaleSlider_;
	Wt::WSlider *transitionScaleSlider_;
	Wt::WText *oneLight_;
	Wt::WText *twoLight_;
	Wt::WText *threeLight_;
	Wt::WText *change_;
	Wt::WText *light_;
	Wt::Http::Client * connect();
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
	void lightOne();
	void lightTwo();
	void lightThree();
	void returnBridge();
	void deleteBridge();
};

#endif
