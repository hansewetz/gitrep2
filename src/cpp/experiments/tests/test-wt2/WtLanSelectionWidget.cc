#include "WtLanSelectionWidget.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WHBoxLayout>
#pragma GCC diagnostic pop
using namespace Wt;
using namespace std;

// ctor
WtLanSelectionWidget::WtLanSelectionWidget(vector<pair<string,string>>const&lanpairs,Wt::WContainerWidget*parent):
    WContainerWidget(parent),selectionChanged_(this),lanpairs_(lanpairs){

  // wrapper around language selection
  WContainerWidget*lanSelectionContainer=new WContainerWidget(this);
  WHBoxLayout*lanSelection=new WHBoxLayout();
  lanSelectionContainer->setLayout(lanSelection);

  // setup source selection
  WContainerWidget*lanContainer=new WContainerWidget();
  lanlist_=new WComboBox();
  for(auto const&lp:lanpairs)lanlist_->addItem(lp.first+"-"+lp.second);
  lanlist_->setCurrentIndex(0);
  lanlist_->changed().connect(this,&WtLanSelectionWidget::lanlistSelectionChanged);
  lanContainer->addWidget(lanlist_);
  lanSelection->addWidget(lanContainer);
}
// get source language
string WtLanSelectionWidget::srclan()const{
  string lanpair=lanlist_->currentText().narrow();
  return lanpair.substr(0,2);
}
// get target language
string WtLanSelectionWidget::trglan()const{
  string lanpair=lanlist_->currentText().narrow();
  return lanpair.substr(3,2);
}
// enable widget
void WtLanSelectionWidget::enable(){
  lanlist_->enable();
}
// disable widget
void WtLanSelectionWidget::disable(){
  lanlist_->disable();
}
// signal emitter when src/target languages changes
Signal<void>&WtLanSelectionWidget::selectionChanged(){
  return selectionChanged_;
}
// source selection changed
void WtLanSelectionWidget::lanlistSelectionChanged(){
  selectionChanged_.emit();
}
