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
	Wt::WText *user_;
	Wt::WPushButton *button;

	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
	Wt::Http::Client * connect();
	void registerBridge();
};


#endif
