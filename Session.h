// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#ifndef SESSION_H_
#define SESSION_H_

#include <vector>
#include <string>

#include <Wt/Auth/Login>

#include <Wt/Dbo/Session>
#include <Wt/Dbo/ptr>
#include <Wt/Dbo/backend/Sqlite3>

#include "User.h"
#include "Light.h"
#include "Bridge.h"
#include "BridgeUserIds.h"

typedef Wt::Auth::Dbo::UserDatabase<AuthInfo> UserDatabase;

typedef Wt::Dbo::collection<Wt::Dbo::ptr<Bridge>> Bridges_Collection;
typedef Wt::Dbo::collection<Wt::Dbo::ptr<BridgeUserIds>> BridgeUserIds_Collection;
typedef Wt::Dbo::ptr<Bridge> BridgePtr;
typedef Wt::Dbo::ptr<User> UserPtr;
typedef Wt::Dbo::ptr<BridgeUserIds> BridgeUserIds_Ptr;



class Session
{
public:
  static void configureAuth();

  Session();
  ~Session();

  Wt::Auth::AbstractUserDatabase& users();
  Wt::Auth::Login& login() { return login_; }


  /*  
   * These methods deal with the currently logged in user
   */
  std::string userName() const;
  std::string firstName() const;
  std::string lastName() const;

  //-------------------------
  //---------User DB--------
  void updateUser(User* newUser);
  User* getUser();

  //-------------------------------------
  //---------Bridge With Users DB--------
  //-------------------------------------
  // ADD
  void addBridgeUserId(Bridge *y, std::string bridgeUserId);  //add to relation table
  // GETTERS
  std::vector<BridgeUserIds> getBridgeUserId();                  //get all the bridgeuserids of currentloy logged in user
  BridgeUserIds* getBridgeUserId(std::string ip, std::string port); //get from specific bridge of currently logged in user
  BridgeUserIds* getBridgeUserId(Bridge *bridgeObj);                //get from specific bridge of currently logged in user

  std::vector<BridgeUserIds> getAllBridgeUserId();                                //NOT IMPLEMENTEDDDDDDD
  std::vector<BridgeUserIds> getAllBridgeUserId(std::string ip, std::string port);//NOT IMPLEMENTEDDDDDDD
  std::vector<BridgeUserIds> getAllBridgeUserId(Bridge *bridgeObj);               //NOT IMPLEMENTEDDDDDDD
  // DELETE
  void deleteBridgeUserId();
  void deleteBridgeUserId(std::string ip, std::string port);
  void deleteBridgeUserId(Bridge *bridgeObj);
  void deleteAllBridgeUserId(); 
  void deleteAllBridgeUserId(std::string ip, std::string port); 
  void deleteAllBridgeUserId(Bridge *bridgeObj); 

  //-------------------------
  //---------Bridge DB--------
  std::vector<Bridge> getBridges();     //currently logged in user
  std::vector<Bridge> getAllBridges();  // all user's bridges

  Bridge* getBridge(std::string ip, std::string port);
  void updateBridge(Bridge* oldBridge, Bridge* newBridge);
  bool addBridge(Bridge* newBridge);
  bool deleteBridge(std::string ip, std::string port);

  //-------------------------
  //---------LIGHT DB--------
  bool setLightBelongsTo(std::string lightName,std::string bridgeIP);
  
  Light* getLight(std::string name);
  void updateLight(Light* newLight);
  bool addLight(Light* newLight);

  //-------------------------
  //-------------------------

  static const Wt::Auth::AuthService& auth();
  static const Wt::Auth::AbstractPasswordService& passwordAuth();
  static const std::vector<const Wt::Auth::OAuthService *>& oAuth();

  Wt::Dbo::ptr<User> user();
  Wt::Dbo::ptr<User> user(const Wt::Auth::User& authUser);

private:
  Wt::Dbo::backend::Sqlite3 sqlite3_;
  mutable Wt::Dbo::Session session_;
  UserDatabase *users_;
  Wt::Auth::Login login_;

  
};

#endif //SESSION_H_
