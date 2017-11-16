#ifndef CUSTOM_AUTH_WIDGET
#define CUSTOM_AUTH_WIDGET

#include <Wt/Auth/AuthWidget>
#include <Wt/WWidget>
#include <Wt/Auth/Identity>

class WWidget;
class Identity;

class CustomAuthWidget : public Wt::Auth::AuthWidget
{
public:
  CustomAuthWidget(Wt::Auth::Login& login, Wt::WContainerWidget *parent = 0);
  Wt::WWidget *createRegistrationView(const Wt::Auth::Identity& id);
};

#endif // QR_AUTH_WIDGET
