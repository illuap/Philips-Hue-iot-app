// SingleGroupsControl.h : Defines the SingleGroupsControl class that manages the deletion, states and changes of states in groups
// States include name, hue, brightness, transition time, saturation, on/off
// Authors: Nicole Chow, Weija Zhou, Paul Li, Daniel Le
// Date: Nov 28, 2017

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <Wt/WContainerWidget>

#ifndef SINGLEGROUPCONTROL_H_
#define SINGLEGROUPCONTROL_H_

class Session;

class SingleGroupsControlWidget: public Wt::WContainerWidget
{
public:
	SingleGroupsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
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
	Wt::WLineEdit *nameEdit_;
	Wt::WText *groupInfoEdit_;
	Wt::WText *groupLightsEdit_;
	Wt::WLineEdit *hueEdit_;
	Wt::WSlider *satScaleSlider_;
	Wt::WSlider *briScaleSlider_;
	Wt::WSlider *hueScaleSlider_;
	Wt::WSlider *transitionScaleSlider_;
	Wt::WText *change_;
	Wt::WComboBox *addChoices_;
	Wt::WComboBox *removeChoices_;
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	void on();
	void off();
	void hue();
	void bright();
	void sat();
	void name();
	static void music(Wt::WSound *sound);
	void addLights();
	void partyMode();
	void sunsetMode();
	void fiftyMode();
	void mustangMode();
	void bloodMode();
	void oceanMode();
	void fireMode();
	void forestMode();
	void removeLights();
	void deleteGroup();
	void returnBridge();
	void transition();
};

#endif
