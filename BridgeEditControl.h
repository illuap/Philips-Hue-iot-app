#include <Wt/WContainerWidget>
#include <boost/lexical_cast.hpp>
#include <boost/system/system_error.hpp>
#include <string>
#include "Bridge.h"


#ifndef BRIDGEEDITCONTROL_H_
#define BRIDGEEDITCONTROL_H_

class Session;

class BridgeEditControlWidget : public Wt::WContainerWidget
{
public:
	BridgeEditControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	void update();

private:
	Session *session_;
	Bridge *thisBridge;
	Wt::WLineEdit *nameEdit_;
	Wt::WLineEdit *ipEdit_;
	Wt::WLineEdit *portEdit_;
	Wt::WLineEdit *locationEdit_;
	Wt::WText *errorText_;
	std::string userId;
	std::string ip = "";
	std::string port = "";
	void updateEdit();
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
};

#endif
