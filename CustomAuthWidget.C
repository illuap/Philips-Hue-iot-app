
/*
 * Copyright (C) 2012 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "CustomAuthWidget.h"

#include <Wt/Auth/RegistrationWidget>
#include <Wt/Auth/RegistrationModel>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WImage>
#include <Wt/WText>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/Utils>
#include <Wt/Auth/Login>


class RegistrationModel;

CustomAuthWidget::CustomAuthWidget(Wt::Auth::Login& login, Wt::WContainerWidget *parent)
  : Wt::Auth::AuthWidget(login, parent)
{ }



Wt::WWidget *CustomAuthWidget::createRegistrationView(const Wt::Auth::Identity& id)
{
  Wt::Auth::RegistrationModel *model = registrationModel();

  if (id.isValid())
    model-> registerIdentified(id);

  Wt::Auth::RegistrationWidget *w = new Wt::Auth::RegistrationWidget(this);
  w->setModel(model);

  return w;
}
