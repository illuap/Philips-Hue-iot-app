#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


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
	Wt::WLineEdit *nameEdit_;
	Wt::WLineEdit *hueEdit_;
	Wt::WSlider *satScaleSlider_;
	Wt::WSlider *briScaleSlider_;
	Wt::WSlider *hueScaleSlider_;
	Wt::WText *change_;
	Wt::WText *light_;
	std::string currentLight = "0";
	void on();
	void off();
	void hue();
	void bright();
	void sat();
	void name();
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void lightOne();
	void lightTwo();
	void lightThree();
	void returnBridge();
};

#endif
