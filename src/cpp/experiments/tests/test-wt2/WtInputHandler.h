// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.
#ifndef WT_INPUT_HANDLER_H__
#define WT_INPUT_HANDLER_H__
#include "WtLanSelectionWidget.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WPushButton>
#include <Wt/WTimer>
#pragma GCC diagnostic pop

// wt input handler class.
class WtInputHandler:public Wt::WApplication{
public:
  // ctor
  WtInputHandler(std::size_t pollms,std::vector<std::string>const&srclans,std::vector<std::string>const&trglans,const Wt::WEnvironment&env);
private:
  // widgets
  WtLanSelectionWidget*lanSelectionWidget_;
  Wt::WPushButton*translateButton_;
  Wt::WFileUpload*fileUploader_;
  Wt::WTextArea*srcTextArea_;
  Wt::WTextArea*trgTextArea_;
  Wt::WTimer*timer_;

  // other state
  bool translating_=false;

  // event handlers
  void lanSelectionChanged();
  void translatePushed();
  //void srcTextChanged();
  void timerPopped();

  // helper functions
  void startTimer();
  void stopTimer();
  void processStateChange();
};
#endif
