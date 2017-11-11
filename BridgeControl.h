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

	Wt::WText *ip_;
	Wt::WLineEdit *ipEdit_;
	Wt::WText *port_;
	Wt::WLineEdit *portEdit_;
	Wt::WText *location_;
	Wt::WLineEdit *locationEdit_;
	Wt::WLineEdit *name_;
	Wt::WPushButton *button;
	Wt::WText *errorText_;

	Wt::WText *confirm_;

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void registerBridge();
	std::string username; 
	std::string ip; 
	std::string port; 
};


#endif