// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef CUSTOM_REGISTRATION_WIDGET_H_
#define CUSTOM_REGISTRATION_WIDGET_H_

//#include <Wt/WTemplateFormView>
//#include <Wt/Auth/RegistrationModel>
#include <Wt/Auth/RegistrationWidget>
#include "CustomAuthWidget.h"

namespace Wt {
  namespace Auth {

class AuthWidget;
class Login;
class OAuthProcess;

/*! \class RegistrationWidget Wt/Auth/RegistrationWidget
 *  \brief A registration widget.
 *
 * This implements a widget which allows a new user to register.  The
 * widget renders the <tt>"Wt.Auth.template.registration"</tt>
 * template. and uses a RegistrationModel for the actual registration
 * logic.
 *
 * Typically, you may want to specialize this widget to ask for other
 * information.
 *
 * \ingroup auth
 */
class CustomRegistrationWidget : public Wt::Auth::RegistrationWidget
{
public:
  CustomRegistrationWidget(AuthWidget *authWidget = 0);

  virtual void update();

};

  }
}

#endif // WT_AUTH_REGISTRATION_WIDGET_H_
