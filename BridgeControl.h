#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>


#ifndef BRIDGECONTROL_H_
#define BRIDGECONTROL_H_

class Session;

class BridgeControlWidget : public Wt::WContainerWidget
{
public:
	BridgeControlWidget(Session *session, WContainerWidget *parent);
	void update();

private:
	Session *session_;
	//Displays the user id, remove later
	Wt::WText *user_;

	Wt::WLineEdit *name_;
	Wt::WText *ipList_;
	Wt::WText *ip_;
	Wt::WLineEdit *ipEdit_;
	Wt::WText *port_;
	Wt::WLineEdit *portEdit_;
	Wt::WPushButton *button;

	//Test. Remove later
	Wt::WText *testText_;

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	void handleHttpResponseIp(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void registerBridge();
	void setIpAddress();
	std::string username; 
	std::string ip; 
	std::string port; 
	void showLights();
};


#endif
