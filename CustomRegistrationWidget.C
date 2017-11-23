/*
 * Copyright (C) 2011 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/Auth/AbstractUserDatabase>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/Login>
#include <Wt/Auth/RegistrationWidget>
#include <Wt/WAnchor>
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WDialog>
#include <Wt/WImage>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WTheme>
#include <Wt/WDllDefs.h>

#include <memory>
#include "CustomRegistrationWidget.h"

#ifdef WT_CXX11
#define AUTO_PTR std::unique_ptr
#else
#define AUTO_PTR std::auto_ptr
#endif

namespace Wt {

  namespace Auth {



RegistrationWidget::RegistrationWidget(AuthWidget *authWidget)
  : WTemplateFormView(tr("Wt.Auth.template.registration")),
    authWidget_(authWidget),
    model_(0),
    created_(false),
    confirmPasswordLogin_(0)
{
  setWidgetIdMode(SetWidgetObjectName);

  WApplication *app = WApplication::instance();
  app->theme()->apply(this, this, AuthWidgets);
}

WWidget *RegistrationWidget::createFormWidget(WFormModel::Field field)
{
  WFormWidget *result = 0;

  return result;
}

void RegistrationWidget::update()
{

}

  }
}
