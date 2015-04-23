#include "WtLanSelectionWidget.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#pragma GCC diagnostic pop
using namespace Wt;
using namespace std;

// ctor
WtLanSelectionWidget::WtLanSelectionWidget(vector<string>const&srclans,vector<string>const&trglans,Wt::WContainerWidget*parent):
    WContainerWidget(parent),selectionChanged_(this),srclans_(srclans),trglans_(trglans){

  // wrapper around language selection
  WContainerWidget*lanSelectionContainer=new WContainerWidget(this);
  WHBoxLayout*lanSelection=new WHBoxLayout();
  lanSelectionContainer->setLayout(lanSelection);

  // setup source selection
  WContainerWidget*srcContainer=new WContainerWidget();
  WVBoxLayout*srcbox=new WVBoxLayout();
  srcContainer->setLayout(srcbox);
  srcbox->addWidget(new WText("source"));
  srcList_=new WComboBox();
  for(auto const&lan:srclans_)srcList_->addItem(lan);
  srcList_->setCurrentIndex(0);
  srcList_->changed().connect(this,&WtLanSelectionWidget::srcListChanged);
  srcbox->addWidget(srcList_);
  lanSelection->addWidget(srcContainer);

  // setup target selection
  WContainerWidget*trgContainer=new WContainerWidget();
  WVBoxLayout*trgbox=new WVBoxLayout();
  trgContainer->setLayout(trgbox);
  trgbox->addWidget(new WText("target"));
  trgList_=new WComboBox();
  for(auto const&lan:trglans_)trgList_->addItem(lan);
  trgList_->setCurrentIndex(0);
  trgList_->changed().connect(this,&WtLanSelectionWidget::trgListChanged);
  trgbox->addWidget(trgList_);
  lanSelection->addWidget(trgContainer);
}
// get source language
string WtLanSelectionWidget::srclan()const{
  return srcList_->currentText().narrow();
}
// get target language
string WtLanSelectionWidget::trglan()const{
  return trgList_->currentText().narrow();
}
// enable widget
void WtLanSelectionWidget::enable(){
  srcList_->enable();
  trgList_->enable();
}
// disable widget
void WtLanSelectionWidget::disable(){
  srcList_->disable();
  trgList_->disable();
}
// signal emitter when src/target languages changes
Signal<void>&WtLanSelectionWidget::selectionChanged(){
  return selectionChanged_;
}
// source selection changed
void WtLanSelectionWidget::srcListChanged(){
  selectionChanged_.emit();
}
// target selection changed
void WtLanSelectionWidget::trgListChanged(){
  selectionChanged_.emit();
}
