/*
 * Copyright (C) 2011 Emweb bvba, Heverlee, Belgium
 *
 * See the LICENSE file for terms of use.
 */

#include "HangmanWidget.h"

#include <Wt/WBreak>
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <boost/lexical_cast.hpp>

#include "Session.h"
#include "ImagesWidget.h"

using namespace Wt;

namespace {
  const int MaxGuesses = 9;
}

HangmanWidget::HangmanWidget(const std::string &name, WContainerWidget *parent)
  : WContainerWidget(parent),
    name_(name),
    badGuesses_(0)
{
  setContentAlignment(AlignCenter);
  
  title_ = new WText(tr("hangman.readyToPlay"), this);

  statusText_ = new WText(this);
  images_ = new ImagesWidget(MaxGuesses, this);

  language_ = new WComboBox(this);
  language_->addItem(tr("hangman.englishWords").arg(18957));

  new WBreak(this);

  newGameButton_ = new WPushButton(tr("hangman.newGame"), this);
  newGameButton_->clicked().connect(this, &HangmanWidget::newGame);
}

void HangmanWidget::newGame()
{
  WString title(tr("hangman.guessTheWord"));
  title_->setText(title.arg(name_));

  language_->hide();
  newGameButton_->hide();

  /*
   * Choose a new secret word and reset the game
   */
  badGuesses_ = 0;
  images_->showImage(badGuesses_);
  statusText_->setText("");
}

void HangmanWidget::registerGuess(char c)
{
  if (badGuesses_ < MaxGuesses) {
    //bool correct = word_->guess(c);

    if (!correct) {
      ++badGuesses_;
      images_->showImage(badGuesses_);
    }
  }
/*
  if (badGuesses_ == MaxGuesses) {
    WString status(tr("hangman.youHang"));
    //statusText_->setText(status.arg(word_->word()));

    language_->show();
    newGameButton_->show();

    scoreUpdated_.emit(-10);
  } else if (word_->won()) {
    statusText_->setText(tr("hangman.youWin"));
    images_->showImage(ImagesWidget::HURRAY);

    language_->show();
    newGameButton_->show();

    scoreUpdated_.emit(20 - badGuesses_);
  }
  */
}
