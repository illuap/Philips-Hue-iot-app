/*
 * Copyright (C) 2012 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#include <Wt/Auth/RegistrationModel>
#include <Wt/WPushButton>
#include "AuthWidget.h"
#include "RegistrationView.h"
#include "Session.h"

AuthWidget::AuthWidget(Session& session)
  : Wt::Auth::AuthWidget(Session::auth(), session.users(), session.login()),
    session_(session)
{  }

Wt::WWidget *AuthWidget::createRegistrationView(const Wt::Auth::Identity& id)
{
  RegistrationView *w = new RegistrationView(session_, this);
  Wt::Auth::RegistrationModel *model = createRegistrationModel();

  if (id.isValid()){
    model->registerIdentified(id);
    model->setEmailPolicy(Wt::Auth::RegistrationModel::EmailPolicy::EmailMandatory);

  }
	

  w->setModel(model);
  return w;
}