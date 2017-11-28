/** @file UserDetailsModel.C
*  @brief adding onto the registration to take in more user details
*
*   Handles the Users who are able to access the database and create a
*   persistent environment for our application using sqlite3. It hashes
*   user passwords and stores tokens. The is also a multitude of functions
*   to help access and store information into the applications database. 
*   Through out the app session is used to authorize each user to a page.
*
*  @author Paul Li
*  @date Nov 28, 2017
*/

#include "UserDetailsModel.h"
#include "User.h"
#include "Session.h"

const Wt::WFormModel::Field 
UserDetailsModel::FirstNameField = "first-name";
const Wt::WFormModel::Field 
UserDetailsModel::LastNameField = "last-name";

/** @brief Constructor for UserDetailsModel
 *
 *  creates a wformmodel to be added onto our registration section to take in first and last name
 *  
 *  @param session the current session data of the app.
 *	@param parent Wobject this wformModel it will belong to.
 */
UserDetailsModel::UserDetailsModel(Session& session, Wt::WObject *parent)
  : Wt::WFormModel(parent),
    session_(session)
{
  addField(FirstNameField, Wt::WString::tr("Enter your first name"));
  addField(LastNameField, Wt::WString::tr("Enter your last name"));
}
/** @brief Save the data that was entered into the form into the db
 *
 *  After this function is called grab the data from the WForm::Field data and store it into their respective
 *	locations in the database.
 *  
 *  @param authUser the auth model of the user.
 */
void UserDetailsModel::save(const Wt::Auth::User& authUser)
{
  Wt::Dbo::ptr<User> user = session_.user(authUser);
  user.modify()->firstName = valueText(FirstNameField).toUTF8();
  user.modify()->lastName = valueText(LastNameField).toUTF8();
}
