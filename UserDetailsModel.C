/*
 * Copyright (C) 2012 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "UserDetailsModel.h"
#include "User.h"
#include "Session.h"

const Wt::WFormModel::Field 
UserDetailsModel::FirstNameField = "first-name";
const Wt::WFormModel::Field 
UserDetailsModel::LastNameField = "last-name";

UserDetailsModel::UserDetailsModel(Session& session, Wt::WObject *parent)
  : Wt::WFormModel(parent),
    session_(session)
{
  addField(FirstNameField, Wt::WString::tr("Enter your first name"));
  addField(LastNameField, Wt::WString::tr("Enter your last name"));
}

void UserDetailsModel::save(const Wt::Auth::User& authUser)
{
  Wt::Dbo::ptr<User> user = session_.user(authUser);
  user.modify()->firstName = valueText(FirstNameField).toUTF8();
  user.modify()->lastName = valueText(LastNameField).toUTF8();
  //user.modify()->email = authUser.unverifiedEmail (); works but we dont needa store it
}
