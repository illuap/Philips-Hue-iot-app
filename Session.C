

#include "Session.h"

#include "Wt/Auth/AuthService"
#include "Wt/Auth/HashFunction"
#include "Wt/Auth/PasswordService"
#include "Wt/Auth/PasswordStrengthValidator"
#include "Wt/Auth/PasswordVerifier"
#include "Wt/Auth/GoogleService"
#include "Wt/Auth/Dbo/AuthInfo"
#include "Wt/Auth/Dbo/UserDatabase"

#include <Wt/WApplication>
#include <Wt/WLogger>

#ifndef WT_WIN32
#include <unistd.h>
#endif
//check if crypting is avalible
#if !defined(WT_WIN32) && !defined(__CYGWIN__) && !defined(ANDROID)
#define HAVE_CRYPT
#endif

using namespace Wt;
namespace dbo = Wt::Dbo;

namespace {

#ifdef HAVE_CRYPT
class UnixCryptHashFunction : public Auth::HashFunction
  {
  public:
    virtual std::string compute(const std::string& msg, 
				const std::string& salt) const
    {
      std::string md5Salt = "$1$" + salt;
      return crypt(msg.c_str(), md5Salt.c_str());
    }

    virtual bool verify(const std::string& msg,
			const std::string& salt,
			const std::string& hash) const
    {
      return crypt(msg.c_str(), hash.c_str()) == hash;
    }

    virtual std::string name () const {
      return "crypt";
    }
  };
#endif // HAVE_CRYPT

  class MyOAuth : public std::vector<const Auth::OAuthService *>
  {
  public:
    ~MyOAuth()
    {
      for (unsigned i = 0; i < size(); ++i)
	delete (*this)[i];
    }
  };

  Auth::AuthService myAuthService;
  Auth::PasswordService myPasswordService(myAuthService);
  MyOAuth myOAuthServices;
}
//
void Session::configureAuth()
{
  myAuthService.setAuthTokensEnabled(true, "hueappcookie");
  myAuthService.setEmailVerificationEnabled(true);
  myAuthService.setIdentityPolicy(Wt::Auth::IdentityPolicy::EmailAddressIdentity);

  Auth::PasswordVerifier *verifier = new Auth::PasswordVerifier();
  verifier->addHashFunction(new Auth::BCryptHashFunction(7));

#ifdef HAVE_CRYPT
  verifier->addHashFunction(new UnixCryptHashFunction());
#endif

  myPasswordService.setVerifier(verifier);
  myPasswordService.setAttemptThrottlingEnabled(true);

  if (Auth::GoogleService::configured())
    myOAuthServices.push_back(new Auth::GoogleService(myAuthService));
}


Session::Session()
  : sqlite3_(WApplication::instance()->appRoot() + "hueApp.db")
{
  session_.setConnection(sqlite3_);
  sqlite3_.setProperty("show-queries", "true");

  session_.mapClass<User>("user");
  session_.mapClass<Light>("light");
  session_.mapClass<Bridge>("bridge");
  session_.mapClass<BridgeUserIds>("BridgeUserIds");
  session_.mapClass<AuthInfo>("auth_info");
  session_.mapClass<AuthInfo::AuthIdentityType>("auth_identity");
  session_.mapClass<AuthInfo::AuthTokenType>("auth_token");
  

  users_ = new UserDatabase(session_);

  dbo::Transaction transaction(session_);
  try {
    session_.createTables();

    /*
     * Add a default guest/guest account
     */
    Auth::User guestUser = users_->registerNew();
    guestUser.addIdentity(Auth::Identity::LoginName, "guest");
    myPasswordService.updatePassword(guestUser, "guest");


    Wt::log("info") << "Database created";
  } catch (...) {
    Wt::log("info") << "Using existing database";
  }

  transaction.commit();
}

Session::~Session()
{
  delete users_;
}

dbo::ptr<User> Session::user()
{
  if (login_.loggedIn()) {
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    dbo::ptr<User> user = authInfo->user();

    if (!user) {
      user = session_.add(new User());
      authInfo.modify()->setUser(user);
      //user.modify()-> name = login_.user().identity(Auth::Identity::LoginName).toUTF8();

    }

    return user;
  } else
    return dbo::ptr<User>();
}

dbo::ptr<User> Session::user(const Wt::Auth::User& authUser)
{
  dbo::ptr<AuthInfo> authInfo = users_->find(authUser);

  dbo::ptr<User> user = authInfo->user();

  if (!user) {
    user = session_.add(new User());
    authInfo.modify()->setUser(user);
  }

  return user;
}

// Function Name: userName()
// Parameters: none
// Return: string
// Description: get the username of the currently logged in user.
std::string Session::userName() const
{
  if (login_.loggedIn())
    return login_.user().identity(Auth::Identity::LoginName).toUTF8();
  else
    return std::string();
}

// Function Name: firstName()
// Parameters: none
// Return: string
// Description: get the first name of the currently logged in user.
std::string Session::firstName() const
{
  if (login_.loggedIn()) {
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    dbo::ptr<User> user = authInfo->user();

    return user->firstName;
  } else
    return std::string();
}

// Function Name: lastName()
// Parameters: none
// Return: string
// Description: get the last name of the currently logged in user.
std::string Session::lastName() const
{
  if (login_.loggedIn()) {
    dbo::ptr<AuthInfo> authInfo = users_->find(login_.user());
    dbo::ptr<User> user = authInfo->user();

    return user->lastName;
  } else
    return std::string();
}
//---------------------

// Function Name: getBridge()
// Parameters: string of ip and string of port for desired bridge in db
// Return: Bridge Object
// Description: Get a bridge from the database with a given ip address and port number.
Bridge* Session::getBridge(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  transaction.commit();
  return bridgeObj.modify();
}

// Function Name: deleteBridge()
// Parameters: string of ip and string of port for desired bridge in db
// Return: boolean if it deleted or not
// Description: Delete a bridge from the database with a given ip address and port number.
bool Session::deleteBridge(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  if(bridgeObj){
    bridgeObj.remove();
  }else{
    Wt::log("info") << "Failed to delete could not find given bridge. ("
                    << ip <<":"<<port << ")";
  }

  transaction.commit();
}

// Function Name: updateBridge()
// Parameters: old bridge object holding the ip/port to change + the newBridge information
// Return: none
// Description: Changes attributes in the ip/port of old bridge with all new attributes from newBridge
void Session::updateBridge(Bridge* oldBridge, Bridge* newBridge){
  dbo::Transaction transaction(session_);
  Wt::log("info") << "xxxxxxxx" << newBridge->getIpAddress() << ":" << newBridge->getPortNumber() ;
 
  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
                .where("ipAddress = ?").bind(oldBridge->getIpAddress())
                .where("portNumber = ?").bind(std::to_string(oldBridge->getPortNumber()));
	  bridgeObj.modify()->setBridgeName(newBridge->getBridgeName());
	  bridgeObj.modify()->setLocation(newBridge->getLocation());
	  bridgeObj.modify()->setIpAddress(newBridge->getIpAddress());
	  bridgeObj.modify()->setHostName(newBridge->getHostName());
	  bridgeObj.modify()->setUserId(newBridge->getUserId());
	  bridgeObj.modify()->setRegistered(newBridge->getRegistered());
	  bridgeObj.modify()->setPortNumber(newBridge->getPortNumber());
 
  transaction.commit();
}

// Function Name: getAllBridges()
// Parameters: none
// Return: a vector of bridge objects
// Description: Get all the bridges in the database.
std::vector<Bridge> Session::getAllBridges(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgePtr> query = session_.find<Bridge>();
  Bridges_Collection bridges = query.resultList();
  std::vector<Bridge> x;
  for (Bridges_Collection::const_iterator i = bridges.begin(); i != bridges.end(); ++i){
    dbo::ptr<Bridge> bridge = *i;
    x.push_back(*bridge);
  }

  transaction.commit();
  return x;
}

// Function Name: getBridges()
// Parameters: none
// Return: a vector of bridge objects
// Description: Get the currently logged in user's bridges in the database.
std::vector<Bridge> Session::getBridges(){
  dbo::Transaction transaction(session_);


  dbo::ptr<User> u = user();
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>().where("userID_id = ?").bind(u.id());
  BridgeUserIds_Collection ids = query.resultList();
  std::vector<Bridge> x;

  for (BridgeUserIds_Collection::const_iterator i = ids.begin(); i != ids.end(); ++i){
    BridgeUserIds_Ptr temp = *i;
    BridgePtr bridgeObj = session_.find<Bridge>().where("id = ?").bind(temp->bridge.id());
    x.push_back(*bridgeObj);
  }

  transaction.commit();
  return x;
}

// Function Name: addBridge()
// Parameters: bridge object to be inserted
// Return: boolean of it failed or not
// Description: add a bridge to the database
bool Session::addBridge(Bridge* newBridge){
  
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj;
    bridgeObj = session_.find<Bridge>().where("ipAddress = ?").bind(newBridge->getIpAddress())
                                        .where("portNumber = ?").bind(newBridge->getPortNumber());
    if(!bridgeObj){
      //Wt::Dbo::ptr<Light> temp = session_.add(new Light("name2","type2",200,201,202,true,203));
      bridgeObj = session_.add(newBridge);
      //bridgeObj.modify()->lights.insert( temp );
      transaction.commit();
      return true;
    }else{
      transaction.commit();
      return false;
    }

  transaction.commit();
}

// Function Name: updateUser()
// Parameters: User object to replace/alter the current one
// Return: none
// Description: updates the current logged in user with a new info.
void Session::updateUser(User* newUser){
  dbo::Transaction transaction(session_);

  dbo::ptr<User> user = session_.find<User>().where("name = ?").bind(userName());
  user.modify()->name = newUser->name;
  user.modify()->firstName = newUser->firstName;
  user.modify()->lastName = newUser->lastName;
  user.modify()->email = newUser->email;
  user.modify()->bridgeUserID = newUser->bridgeUserID;

  transaction.commit();
}
// Function Name: getAllBridges()
// Parameters: none
// Return: User object of currently logged in user
// Description: Getter function for the user object of the currently logged in user.
User* Session::getUser(){
  dbo::Transaction transaction(session_);
  dbo::ptr<User> user = session_.find<User>().where("name = ?").bind(userName());
  transaction.commit();
  return user.modify();
}

  //-------------------------------------
  //---------Bridge With Users DB--------
  //-------------------------------------
// Function Name: getAllBridges()
// Parameters: none
// Return: a vector of bridge objects
// Description: Get all the bridges in the database.
void Session::addBridgeUserId(Bridge *y, std::string bridgeUserId){
  dbo::Transaction transaction(session_);
  
  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(y->getIpAddress())
            .where("portNumber = ?").bind(y->getPortNumber());
  if(!bridgeObj){
    Wt::log("info") << "Adding BridgeUserId failed. Bridge doesnt' exist";
    transaction.commit();
    return;
  }
  // check there are none in the db
  int count = session_.query<int>("select count(1) from BridgeUserIds")
                    .where("bridgeID_id = ?").bind(bridgeObj.id())
                    .where("userID_id = ?").bind(this->user().id());

  if(count > 0){
    Wt::log("info") << "Adding BridgeUserId failed. BridgeUserId already exists!";
    transaction.commit();
    return;
  }

  BridgeUserIds *temp = new BridgeUserIds(user(), bridgeObj,bridgeUserId);
  BridgeUserIds_Ptr x = session_.add(temp);

  transaction.commit();
}

//======GETTERS======
// Function Name: getBridgeUserId()
// Parameters: none
// Return: a vector of bridgeUserIds objects
// Description: get a vector of all the BridgeUserIds for the currently logged in user.
std::vector<BridgeUserIds> Session::getBridgeUserId(){
  dbo::Transaction transaction(session_);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("userID_id = ?").bind(this->user().id());
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }

  transaction.commit();
  return x;
}

// Function Name: getBridgeUserId()
// Parameters: string of ip and the string of the port of a desired bridge
// Return: a bridgeUserIds objects
// Description: Get the object of a specific bridgeUserId for currently logged in user.
BridgeUserIds* Session::getBridgeUserId(std::string ip, std::string port){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId obtained: "<< y.modify()->bridgeUserID;
                            
  transaction.commit();
  return y.modify();
}

// Function Name: getBridgeUserId()
// Parameters: bridge object of desired bridge
// Return: a bridgeUserIds objects
// Description: Get the object of a specific bridgeUserId for currently logged in user.
BridgeUserIds* Session::getBridgeUserId(Bridge *bridgeObj){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId obtained: "<< y.modify()->bridgeUserID;

  transaction.commit();
  return y.modify();
}

std::vector<BridgeUserIds> Session::getAllBridgeUserId(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>();
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;
  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }

  transaction.commit();
  return x;
}
std::vector<BridgeUserIds> Session::getAllBridgeUserId(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }
  transaction.commit();
  return x;
}

std::vector<BridgeUserIds> Session::getAllBridgeUserId(Bridge *bridgeObj){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    x.push_back(*y);
  }
  transaction.commit();
  return x;
}


void Session::deleteBridgeUserId(){
  dbo::Transaction transaction(session_);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("userID_id = ?").bind(this->user().id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleting BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}

void Session::deleteBridgeUserId(std::string ip, std::string port){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());
  Wt::log("info") << "BridgeUserId deleted: "<< y.modify()->bridgeUserID;
  y.remove();  
  transaction.commit();
}
void Session::deleteBridgeUserId(Bridge *bridgeObj){

  dbo::Transaction transaction(session_);
  Wt::log("info") << "Function getBridgeUserId was called";
  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));

  dbo::ptr<User> current_user = this->user();

  Wt::log("info") << "Bridge ID obtained: " << temp_bridge.id();
  Wt::log("info") << "User ID obtained: "<< current_user.id();

  BridgeUserIds_Ptr y = session_.find<BridgeUserIds>()
                            .where("bridgeID_id = ?").bind(temp_bridge.id())
                            .where("userID_id = ?").bind(current_user.id());

  Wt::log("info") << "BridgeUserId deleted: "<< y.modify()->bridgeUserID;
  y.remove();
  transaction.commit();
}


void Session::deleteAllBridgeUserId(){
  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>();
  BridgeUserIds_Collection temp = query.resultList();
  std::vector<BridgeUserIds> x;
  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleted BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}
void Session::deleteAllBridgeUserId(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Deleted BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}

void Session::deleteAllBridgeUserId(Bridge *bridgeObj){
  dbo::Transaction transaction(session_);

  BridgePtr temp_bridge = session_.find<Bridge>()
            .where("ipAddress = ?").bind(bridgeObj->getIpAddress())
            .where("portNumber = ?").bind(std::to_string(bridgeObj->getPortNumber()));
  Wt::Dbo::Query<BridgeUserIds_Ptr> query = session_.find<BridgeUserIds>()
            .where("bridgeID_id = ?").bind(temp_bridge.id());
  BridgeUserIds_Collection temp = query.resultList();

  for (BridgeUserIds_Collection::const_iterator i = temp.begin(); i != temp.end(); ++i){
    BridgeUserIds_Ptr y = *i;
    Wt::log("info") << "Removed BridgeUserIds (id, user_id,  bridge_id): "
                    << y->bridgeUserID << " , " 
                    << y->user.id() << " , " 
                    << y->bridge.id() ;
    y.remove();
  }
  transaction.commit();
}











bool Session::setLightBelongsTo(std::string lightName,std::string bridgeIP){
  
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj;
  bridgeObj = session_.find<Bridge>().where("ipAddress = ?").bind(bridgeIP);

  dbo::ptr<Light> lightObj;
  lightObj = session_.find<Light>().where("name = ?").bind(lightName);
    if(!bridgeObj && !lightObj){
      bridgeObj.modify()->lights.insert(lightObj);
      return true;
    }else{
      std::cerr << "Setting light belonging to bridge." << std::endl;
      return false;
    }

  transaction.commit();
}

//---------------------
//---------------------
Light* Session::getLight(std::string name){
  dbo::Transaction transaction(session_);

  dbo::ptr<Light> lightObj = session_.find<Light>().where("name = ?").bind(name);
  
  transaction.commit();
  return lightObj.modify();
}

void Session::updateLight(Light* newLight){
  dbo::Transaction transaction(session_);

  dbo::ptr<Light> lightObj = session_.find<Light>().where("name = ?").bind(newLight->getName());
  
  lightObj.modify()->setName(newLight->getName());
  lightObj.modify()->setType(newLight->getType());
  lightObj.modify()->setBrightness(newLight->getBrightness());
  lightObj.modify()->setHue(newLight->getHue());
  lightObj.modify()->setSaturation(newLight->getSaturation());
  lightObj.modify()->setOn(newLight->getOn());
  lightObj.modify()->setTransition(newLight->getTransition());

  transaction.commit();
}

bool Session::addLight(Light* newLight){
  
  dbo::Transaction transaction(session_);

  dbo::ptr<Light> lightObj;
    lightObj = session_.find<Light>().where("name = ?").bind(newLight->getName());
    if(!lightObj){
      lightObj = session_.add(newLight);
      return true;
    }else{
      return false;
    }

  transaction.commit();
}

Auth::AbstractUserDatabase& Session::users()
{
  return *users_;
}

const Auth::AuthService& Session::auth()
{
  return myAuthService;
}

const Auth::AbstractPasswordService& Session::passwordAuth()
{
  return myPasswordService;
}

const std::vector<const Auth::OAuthService *>& Session::oAuth()
{
  return myOAuthServices;
}
