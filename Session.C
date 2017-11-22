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

void Session::configureAuth()
{
  myAuthService.setAuthTokensEnabled(true, "hueappcookie");
  myAuthService.setEmailVerificationEnabled(true);
  //myAuthService.setEmailPolicy(Wt::Auth::EmailPolicy::Mandatory)

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

    Light *light1 = new Light("name1","type1",200,201,202,true,203);
    dbo::ptr<Light> light1ptr = session_.add(light1);

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

std::string Session::userName() const
{
  if (login_.loggedIn())
    return login_.user().identity(Auth::Identity::LoginName).toUTF8();
  else
    return std::string();
}

//---------------------

Bridge* Session::getBridge(std::string ip, std::string port){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(ip)
            .where("portNumber = ?").bind(port);

  transaction.commit();
  return bridgeObj.modify();
}
/*
void Session::addUserBridgeID(std::string newBridgeUserId){
  dbo::Transaction transaction(session_);

  dbo::ptr<User> u = user();
  if (u) {
    u.modify()->name = userName();
    u.modify()->bridgeUserID = newBridgeUserId;
  }

  transaction.commit();
}

std::string Session::getUserBridgeID(){

  dbo::ptr<User> u = user();
  if (u) {

    return u.modify()->bridgeUserID;
  }

}
*/

void Session::updateBridge(Bridge* newBridge){
  dbo::Transaction transaction(session_);

  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>().where("ipAddress = ?").bind(newBridge->getIpAddress());

  bridgeObj.modify()->setBridgeName(newBridge->getBridgeName());
  bridgeObj.modify()->setLocation(newBridge->getLocation());
  bridgeObj.modify()->setIpAddress(newBridge->getIpAddress());
  bridgeObj.modify()->setHostName(newBridge->getHostName());
  bridgeObj.modify()->setUserId(newBridge->getUserId());
  bridgeObj.modify()->setRegistered(newBridge->getRegistered());
  bridgeObj.modify()->setPortNumber(newBridge->getPortNumber());

  transaction.commit();
}

std::vector<Bridge> Session::getBridges(){
  dbo::Transaction transaction(session_);


  dbo::ptr<User> u = user();
  if (u && u.modify()->name == "") {
    u.modify()->name = userName();
  }

  Wt::Dbo::Query<BridgePtr> query = session_.find<Bridge>();
  Bridges bridges = query.resultList();
  std::vector<Bridge> x;
  for (Bridges::const_iterator i = bridges.begin(); i != bridges.end(); ++i){
    dbo::ptr<Bridge> bridge = *i;
    x.push_back(*bridge);
  }

  transaction.commit();
  return x;
}


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


void Session::updateUser(User* newUser){
  dbo::Transaction transaction(session_);

  dbo::ptr<User> user = session_.find<User>().where("name = ?").bind(userName());
  user.modify()->name = newUser->name;
  user.modify()->firstName = newUser->firstName;
  user.modify()->lastName = newUser->lastName;
  user.modify()->email = newUser->email;
  user.modify()->bridgeUserID = newUser->bridgeUserID;
  //user.modify()->bridge = newUser->bridge;

  transaction.commit();
}
/*
void Session::setUserBelongsTo(Bridge* x){
  dbo::Transaction transaction(session_);
  dbo::ptr<Bridge> aBridge =  session_.add(x);
  //dbo::ptr<Bridge> aBridge = session_.find<Bridge>().where("ipAddress = ?").bind(x->getIpAddress())
  //.where("portNumber = ?").bind(x->getPortNumber());
  dbo::ptr<User> user = session_.find<User>().where("name = ?").bind(userName());

  if(aBridge && user){
    user.modify()->bridges.insert(aBridge);
  }

  transaction.commit();
}
*/

User* Session::getUser(){
  dbo::Transaction transaction(session_);
  dbo::ptr<User> user = session_.find<User>().where("name = ?").bind(userName());
  transaction.commit();
  return user.modify();
}


void Session::addBridgeUserId(Bridge *y, std::string bridgeUserId){
  dbo::Transaction transaction(session_);
  
  dbo::ptr<Bridge> bridgeObj = session_.find<Bridge>()
            .where("ipAddress = ?").bind(y->getIpAddress())
            .where("portNumber = ?").bind(y->getPortNumber());


  BridgeUserIds *temp = new BridgeUserIds(user(), bridgeObj,bridgeUserId);
  dbo::ptr<BridgeUserIds> x = session_.add(temp);

  transaction.commit();
}

/*

std::vector<BridgeUserIds> Session::getBridgeUserId(){

  dbo::Transaction transaction(session_);

  Wt::Dbo::Query<Wt::Dbo::ptr<BridgeUserIds>> query = session_.find<BridgeUserIds>();
  BridgeUserIds temp = query.resultList();
  std::vector<BridgeUserIds> x;
  for (Wt::Dbo::collection<BridgeUserIds>::const_iterator i = temp.begin(); i != temp.end(); ++i){
    dbo::ptr<BridgeUserIds> y = *i;
    x.push_back(*y);
  }

  transaction.commit();
  return x;
}

*/


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
/*
bool Session::addLight(Light* newLight, std::string bridgeIP){
  
  dbo::Transaction transaction(session_);

  dbo::ptr<Light> lightObj;
  dbo::ptr<Bridge> bridgeObj;
    lightObj = session_.find<Light>().where("name = ?").bind(newLight->getName());
    bridgeObj = session_.find<Bridge>().where("ipAddress = ?").bind(bridgeIP);
    if(!lightObj && !bridgeObj ){
      lightObj->
      lightObj = session_.add(newLight);
      return true;
    }else{
      std::cerr << "Error adding newLight to DB (might exist already)" << std::endl;
      return false;
    }

  transaction.commit();
}*/

//---------------------
/*
std::vector<User> Session::topUsers(int limit){
  dbo::Transaction transaction(session_);

  Users top = session_.find<User>().orderBy("score desc").limit(limit);

  std::vector<User> result;
  for (Users::const_iterator i = top.begin(); i != top.end(); ++i) {
    dbo::ptr<User> user = *i;
    result.push_back(*user);
 
    dbo::ptr<AuthInfo> auth = *user->authInfos.begin();
    std::string name = auth->identity(Auth::Identity::LoginName).toUTF8();

    result.back().name = name;
  }

  transaction.commit();

  return result;
}

int Session::findRanking()
{
  dbo::Transaction transaction(session_);
  
  dbo::ptr<User> u = user();
  int ranking = -1;

  if (u)
    ranking = session_.query<int>("select distinct count(score) from user")
      .where("score > ?").bind(u->score);

  transaction.commit();
  
  return ranking + 1;
}
*/
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
