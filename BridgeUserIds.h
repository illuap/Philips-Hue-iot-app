// this file is unused in stage 2 and will be used later on.

#ifndef BRIDGE_USER_IDS_H
#define  BRIDGE_USER_IDS_H

#include <Wt/Dbo/Types>
#include <Wt/Dbo/WtSqlTraits>
#include <Wt/Auth/Dbo/AuthInfo>

#include <string>

class BridgeUserIds
{
public:
  std::string bridgeUserID;
  std::string username;
  std::string ip;
  int port;
  BridgeUserIds(){
    bridgeUserID = "";
    username = "";
    ip = "";
    port = 0;
  }

  BridgeUserIds(std::string id, std::string newUsername, std::string newIP, int newPort){
    bridgeUserID = id;
    username = newUsername;
    ip = newIP;
    port = newPort;
  }

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a, bridgeUserID, "bridgeUserID");
    Wt::Dbo::field(a, username, "username");
    Wt::Dbo::field(a, ip, "ip");
    Wt::Dbo::field(a, port, "port");
  }
};


#endif
