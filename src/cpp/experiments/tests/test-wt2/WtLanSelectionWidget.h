// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.
#ifndef __LAN_SELECTION_WIDGET_H__
#define __LAN_SELECTION_WIDGET_H__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>
#pragma GCC diagnostic pop
#include <string>
#include <vector>
#include <string>

// wt input handler class.
class WtLanSelectionWidget:public Wt::WContainerWidget{
public:
  // ctors etc.
  WtLanSelectionWidget(std::vector<std::string>const&srclans,std::vector<std::string>const&trglans,Wt::WContainerWidget*parent=0);
  WtLanSelectionWidget(WtLanSelectionWidget const&)=delete;
  WtLanSelectionWidget(WtLanSelectionWidget&&)=delete;
  WtLanSelectionWidget&operator=(WtLanSelectionWidget const&)=delete;
  WtLanSelectionWidget&operator=(WtLanSelectionWidget&&)=delete;
  ~WtLanSelectionWidget()=default;

  // getters
  std::string srclan()const;
  std::string trglan()const;

  // enable/disable widget
  void enable();
  void disable();

  // signal emitter when src/target languages changes
  Wt::Signal<void>&selectionChanged();
private:
  // state
  Wt::WComboBox*srcList_;
  Wt::WComboBox*trgList_;

  // signal
  Wt::Signal<void>selectionChanged_;

  // source and target language
  std::vector<std::string>const srclans_;
  std::vector<std::string>const trglans_;

  // event handlers
  void srcListChanged();
  void trgListChanged();
};
#endif
