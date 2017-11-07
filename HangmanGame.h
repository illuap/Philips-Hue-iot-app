// This may look like C code, but it's really -*- C++ -*-
/* 
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include <Wt/WContainerWidget>

#include "Session.h"
#include "LightsControl.h"

#ifndef HANGMANGAME_H_
#define HANGMANGAME_H_




namespace Wt {
  class WStackedWidget;
  class WAnchor;
}

class HangmanWidget;
class HighScoresWidget;
class Session;
class LightsControlWidget; 

class HangmanGame : public Wt::WContainerWidget
{
public:
  HangmanGame(Wt::WContainerWidget *parent = 0);

  void handleInternalPath(const std::string &internalPath);

private:
  Wt::WStackedWidget *mainStack_;
  HangmanWidget *game_;
  LightsControlWidget *the_Lights; 
  Wt::WContainerWidget *links_;
  Wt::WAnchor *backToGameAnchor_;
  Wt::WAnchor *scoresAnchor_;
  Wt::WAnchor *hueLights_;


  Session session_;

  void onAuthEvent();
  void showGame();
  void showHighScores();
  void showLights();

};

#endif //HANGMANGAME_H_
