// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#define WT_INPUT_HANDLER_H__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WComboBox>
#include <Wt/WPushButton>
#include <Wt/WFileUpload>
#pragma GCC diagnostic pop

// wt input handler class.
class WtInputHandler:public Wt::WApplication{
public:
  // ctor
  WtInputHandler(const Wt::WEnvironment&env);
private:
  // state
  Wt::WComboBox*srcList_;
  Wt::WComboBox*trgList_;
  Wt::WPushButton*translateButton_;
  Wt::WFileUpload*fileUploader_;
  bool validLanpair_=false;
  bool haveFiles_=false;

  // event handlers
  void srcListChanged();
  void trgListChanged();
  void translatePushed();
  void uploaded();
  void uploadFileTooLarge();
  
  // helper functions
  void enableHelper();
};
#endif
