/** @file LightsControl.h
*  @class LightsControl
*  @brief Function prototypes for the LightsControl class that manages the states and changes of states in lights
*  @author Nicole Chow
*  @author Weija Zhou
*  @author Paul Li
*  @author Daniel Le
*  @date Nov 28, 2017
*/

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

  /** @fn LightsControlWidget(Session *session, Wt::WContainerWidget *parent = 0)
  *  @brief creates a LightsControlWidget
  *  @param session the database session
  *  @param parent the parent Widget Container
  *  @return LightsControlWidget
  */
  LightsControlWidget(Session *session /*!< database session */, Wt::WContainerWidget *parent = 0 /*!< parent Widget Container */);									
  
  /** @fn void update()
  *  @brief loads LightsControlWidget page
  *  @return Void
  */
  void update();												

private:
	Session *session_;									/*!< keeps track of light status */
	std::string currentLight = "0";						/*!< the light that is currently being changed */
	std::string ip = "";								/*!< bridge's IP address */
	std::string userID = "";							/*!< user's bridge ID */
	std::string port = "";								/*!< bridge's port number */
	Wt::WLineEdit *nameEdit_;							/*!< light's name to be changed */
	Wt::WSlider *satScaleSlider_;						/*!< light's saturation selection */
	Wt::WSlider *briScaleSlider_;						/*!< light's brightness selection */
	Wt::WSlider *hueScaleSlider_;						/*!< light's hue selection */
	Wt::WSlider *transitionScaleSlider_;				/*!< light's transition time selection */
	Wt::WText *oneLight_;								/*!< light 1's name */
	Wt::WText *twoLight_;								/*!< light 2's name */
	Wt::WText *threeLight_;								/*!< light 3's name */
	Wt::WText *change_;									/*!< status of a light change */
	Wt::WText *light_;									/*!< displays the light being changed */
	
	/** @fn Wt::Http::Client * connect()
	*  @brief creates an HTTP client
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();

	/** @fn	void on()
	*  @brief turns a light on
	*  @return Void
	*/
	void on();

	/** @fn void off()
	*  @brief turns a light off
	*  @return Void
	*/
	void off();	

	/** @fn void hue()
	*  @brief changes a light's hue
	*  @return Void
	*/
	void hue();	

	/** @fn void bright()	
	*  @brief changes a light's brightness
	*  @return Void
	*/
	void bright();	

	/** @fn void sat()	
	*  @brief changes a light's saturation
	*  @return Void
	*/
	void sat();	

	/** @fn void name()
	*  @brief changes a light's name
	*  @return Void
	*/
	void name();

	/** @fn void transition() 
	*  @brief changes a light's transition time
	*  @return Void
	*/
	void transition();	

	/** @fn void lightOne()
	*  @brief selects light 1 to change
	*  @return Void
	*/
	void lightOne();

	/** @fn void lightTwo()
	*  @brief selects light 2 to change
	*  @return Void
	*/
	void lightTwo();

	/** @fn void lightThree()
	*  @brief selects light 3 to change
	*  @return Void
	*/
	void lightThree();

	/** @fn void returnBridge()
	*  @brief returns user to the bridge page
	*  @return Void
	*/
	void returnBridge();

	/** @fn void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response)
	*  @brief handles response and displays light information
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err /*!< error code */, const Wt::Http::Message& response /*!< response */);
	
	/** @fn void handleHttpResponseName(boost::system::error_code err, const Wt::Http::Message& response)
	*  @brief handles response for light name changes
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseName(boost::system::error_code err /*!< error code */, const Wt::Http::Message& response /*!< response */);
	
	/** @fn void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response)
	*  @brief handles response and does nothing
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err /*!< error code */, const Wt::Http::Message& response /*!< response */);
};

#endif
