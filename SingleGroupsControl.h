/** @file SingleGroupsControl.h
*  @brief Function prototypes for the SingleGroupsControl class that manages the deletion, states and changes of states in groups
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

#ifndef SINGLEGROUPCONTROL_H_
#define SINGLEGROUPCONTROL_H_

class Session;

class SingleGroupsControlWidget: public Wt::WContainerWidget
{
public:

	/** @brief creates a SingleGroupsControlWidget
	*  @param session the database session
	*  @param parent the parent Widget Container
	*  @return SingleGroupsControlWidget
	*/
	SingleGroupsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	
	/** @brief loads SingleGroupsControlWidget page
	*  @return Void
	*/
	void update();

private:
	Session *session_;												/*!< keeps track of group status */
	std::string ip = "";											/*!< bridge's IP address */
	std::string userID = "";										/*!< user's bridge ID */
	std::string port = "";											/*!< bridge's port number */
	std::string groupID = "";										/*!< group's ID */
	std::string lights;												/*!< group's lights */
	bool deleteConfirm;												/*!< confirmation of intent to delete group */		
	Wt::WLineEdit *nameEdit_;										/*!< groups' name to be changed */
	Wt::WText *groupInfoEdit_;										/*!< group name display */
	Wt::WText *groupLightsEdit_;									/*!< group's light display */
	Wt::WSlider *satScaleSlider_;									/*!< group's saturation to change into */
	Wt::WSlider *briScaleSlider_;									/*!< group's brightness to change into */
	Wt::WSlider *hueScaleSlider_;									/*!< group's hue to change into */								
	Wt::WSlider *transitionScaleSlider_;							/*!< group's transition time to change into */
	Wt::WText *change_;												/*!< status of a group change */
	Wt::WComboBox *addChoices_;										/*!< selected light to add to group */
	Wt::WComboBox *removeChoices_;									/*!< selected light to remove from group */
	
	/** @brief creates an HTTP client
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();

	/** @brief plays party.wav for party mode
	*  @param sound audio file
	*  @return Void
	*/
	static void music(Wt::WSound *sound);

	/** @brief turns group's lights on
	*  @return Void
	*/
	void on();

	/** @brief turns group's lights off
	*  @return Void
	*/
	void off();

	/** @brief changes group's hue
	*  @return Void
	*/
	void hue();

	/** @brief changes group's brightness
	*  @return Void
	*/
	void bright();

	/** @brief changes group's saturation
	*  @return Void
	*/
	void sat();

	/** @brief changes group's name
	*  @return Void
	*/
	void name();

	/** @brief changes group's transition time
	*  @return Void
	*/
	void transition();

	/** @brief adds light to group
	*  @return Void
	*/
	void addLights();

	/** @brief removes light from group
	*  @return Void
	*/
	void removeLights();

	/** @brief puts group on party mode for 10s (color looping). Must wait for party mode to finish. 
	*  @return Void
	*/
	void partyMode();

	/** @brief puts group on sunset mode (yellows)
	*  @return Void
	*/
	void sunsetMode();

	/** @brief puts group on fifty mode (greys)
	*  @return Void
	*/
	void fiftyMode();


	/** @brief puts group on mustang mode (purples)
	*  @return Void
	*/
	void mustangMode();


	/** @brief puts group on blood mode (reds)
	*  @return Void
	*/
	void bloodMode();


	/** @brief puts group on ocean mode (blues)
	*  @return Void
	*/
	void oceanMode();


	/** @brief puts group on fire mode (oranges)
	*  @return Void
	*/
	void fireMode();


	/** @brief puts group on forest mode (greens)
	*  @return Void
	*/
	void forestMode();


	/** @brief deletes a group
	*  @return Void
	*/
	void deleteGroup();


	/** @brief returns user to bridge page
	*  @return Void
	*/
	void returnBridge();

	/** @brief handles response and displays group information
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief handles response reloads page
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseUpdate(boost::system::error_code err, const Wt::Http::Message& response);

	/** @brief handles response and does nothing
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
};

#endif
