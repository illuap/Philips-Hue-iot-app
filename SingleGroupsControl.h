#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


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
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void on();
	void off();
	void hue();
	void bright();
	void transition();
	void sat();
	void name();
	void addLights();
	void removeLights();
	void deleteGroup();
	void returnBridge();
	bool deleteConfirm;
	Wt::WLineEdit *nameEdit_;
	Wt::WText *groupInfoEdit_;
	Wt::WText *groupLightsEdit_;
	Wt::WLineEdit *hueEdit_;
	Wt::WSlider *satScaleSlider_;
	Wt::WSlider *briScaleSlider_;
	Wt::WSlider *hueScaleSlider_;
	Wt::WSlider *transitionScaleSlider_;
	Wt::WText *change_;

};

#endif
