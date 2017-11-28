// This may look like C code, but it's really -*- C++ -*-
/* 
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#ifndef USER_H_
#define USER_H_

#include <Wt/WDateTime>
#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Auth/Dbo/AuthInfo>

#include <string>
#include "Bridge.h"

class User;
class Bridge;

typedef Wt::Auth::Dbo::AuthInfo<User> AuthInfo;
typedef Wt::Dbo::collection< Wt::Dbo::ptr<User> > Users;

class User
{
public:
  User();

  std::string name; /* a copy of auth info's user name */
  std::string firstName;
  std::string lastName;
  std::string email;
  std::string bridgeUserID;
  Wt::Dbo::collection<Wt::Dbo::ptr<Bridge> > bridges;
  Wt::Dbo::collection< Wt::Dbo::ptr<AuthInfo> > authInfos;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, name, "name");
    Wt::Dbo::field(a, firstName, "firstName");
    Wt::Dbo::field(a, lastName, "lastName");
    Wt::Dbo::field(a, email, "email");
    Wt::Dbo::field(a, bridgeUserID, "bridgeUserID");
    Wt::Dbo::hasMany(a, bridges, Wt::Dbo::ManyToMany, "bridge_user");
    Wt::Dbo::hasMany(a, authInfos, Wt::Dbo::ManyToOne, "user");
  }
};

DBO_EXTERN_TEMPLATES(User);

#endif // USER_H_
