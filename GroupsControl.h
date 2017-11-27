/** @file GroupsControl.h
*  @brief Function prototypes for the GroupsControl class that manages the creation and listing of groups
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

#ifndef GROUPCONTROL_H_
#define GROUPCONTROL_H_

class Session;

class GroupsControlWidget: public Wt::WContainerWidget
{
public:

	/** @brief creates a GroupsControlWidget
	*  @param session the database session
	*  @param parent the parent Widget Container
	*  @return GroupsControlWidget
	*/
	GroupsControlWidget(Session *session, Wt::WContainerWidget *parent = 0);
	
	/** @brief loads SingleGroupsControlWidget page
	*  @return Void
	*/
	void update();

private:
	Session *session_;										/*!< keeps track of group status */
	std::string ip = "";									/*!< bridge's IP address */
	std::string userID = "";								/*!< user's bridge ID */
	std::string port = "";									/*!< bridge's port number */
	bool one = false;										/*!< status of light 1's membership in the new group */
	bool two = false;										/*!< status of light 2's membership in the new group */
	bool three = false;										/*!< status of light 3's membership in the new group */
	Wt::WLineEdit *nameEdit_;								/*!< new group's name */
	Wt::WText *light1_;										/*!< displays whether light 1 is part of the new group */
	Wt::WText *light2_;										/*!< displays whether light 2 is part of the new group */
	Wt::WText *light3_;										/*!< displays whether light 3 is part of the new group */
	Wt::WText *status_;										/*!< status of creating a group */

	/** @brief creates an HTTP Client
	*  @return Http::Client
	*/
	Wt::Http::Client * connect();

	/** @brief selects light 1 to be added to the group
	*  @return Void
	*/
	void lightOne();

	/** @brief selects light 2 to be added to the group
	*  @return Void
	*/
	void lightTwo();

	/** @brief selects light 3 to be added to the group
	*  @return Void
	*/
	void lightThree();

	/** @brief creates a new group
	*  @return Void
	*/
	void createGroup();

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

	/** @brief handles response and does nothing
	*  @param err the response's error code
	*  @param response the response
	*  @return Void
	*/
	void handleHttpResponseVOID(boost::system::error_code err, const Wt::Http::Message& response);
};

#endif
