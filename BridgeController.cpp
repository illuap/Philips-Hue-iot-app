#include <stdio.h>
#include <iostream>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WServer>
#include <Wt/Http/Message>
#include <Wt/Http/Client>
#include <Wt/Json/Value>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include "MyResource.h"
#include "log.h"
#include "Bridge.h"

using namespace Wt;
using namespace std;


class BridgeApplication : public WApplication
{
public:
	BridgeApplication(const WEnvironment& env);

private:
	WText *user_;

	void handleHttpResponse(boost::system::error_code err, const Http::Message& response);
	Http::Client * connect();
	void registerBridge();
};


BridgeApplication::BridgeApplication(const WEnvironment& env)
	: WApplication(env)
{

	setTitle("3307 HUE");	

	WPushButton *button
		= new WPushButton("Register", root());										// 1st light button
	button->setMargin(5, Left);
	root()->addWidget(new WBreak());
	user_ = new WText(root());
	user_->setText("test");
													

	button->clicked().connect(this, &BridgeApplication::registerBridge);

	(boost::bind(&BridgeApplication::registerBridge, this));
	(boost::bind(&BridgeApplication::handleHttpResponse, this));
	(boost::bind(&BridgeApplication::connect, this));
}

//creates a client
Http::Client * BridgeApplication::connect() {
	Http::Client *client = new Http::Client(this);
	client->setTimeout(15);
	client->setMaximumResponseSize(10 * 1024);
}

//handles get lights request - creates lights and puts them into database
void BridgeApplication::handleHttpResponse(boost::system::error_code err, const Http::Message& response) {
	WApplication::instance()->resumeRendering();
	if (!err && response.status() == 200) {
		//Parse the username from the response JSON
		size_t pos = response.body().find("username");
		string substring = response.body().substr(pos+11);
		size_t end = substring.find("\"");
		string username = substring.substr(0,end);
		//Test output. Remove later
		user_->setText(username);
		
		
		//Get Bridge from database
		//Set registered status to true
		//Set userId to userId received from response

		/*Bridge* bridge = new Bridge();
		bridge.setRegistered(true);
		bridge.setUserId(username);*/
	}
}



void BridgeApplication::registerBridge(){
	Http::Client *client = BridgeApplication::connect();
		 Http::Message *msg = new Http::Message();
		 msg->addBodyText("{\"devicetype\" : \"danny\"}");
		 client->done().connect(boost::bind(&BridgeApplication::handleHttpResponse, this, _1, _2));
		 client->post("http://localhost:8000/api", *msg);
		 

}



WApplication *createApplication(const WEnvironment& env)
{
	return new BridgeApplication(env);
}

int main(int argc, char **argv)
{
	//from the helper link on OWL
	WLogger logger;
	configLogger(logger);
	try {
		WServer server(argv[0], "");
		try {
			server.setServerConfiguration(argc, argv);

			//from the helper link on OWL
			Wt::WServer server(argv[0]);
			server.setServerConfiguration(argc, argv, WTHTTP_CONFIGURATION);
			server.addResource(new MyResource, "/people");
			server.addEntryPoint(Wt::Application, createApplication);

			info(logger, "Starting resource server.");
			if (server.start()) {
				WServer::waitForShutdown();
				server.stop();
			}
			else {
				fatal(logger, "Fatal error starting resource server.");
				return 1;
			}
			return 0;
		}
		catch (std::exception& e) {
			fatal(logger, "Fatal error starting resource server.", e.what());
			return 1;
		}
	}
	catch (WServer::Exception& e) {
		fatal(logger, "Fatal error creating WServer.", e.what());
		return 1;
	}
	catch (exception& e) {
		fatal(logger, "Fatal error occurred.", e.what());
		return 1;
	}
}
