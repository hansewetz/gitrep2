// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.
#include "WtInputHandler.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WPushButton>
#include <Wt/WBorderLayout>
#include <Wt/WTextArea>
#include <Wt/WTimer>
#pragma GCC diagnostic pop
#include <iostream>
#include <string>
#include <vector>
using namespace Wt;
using namespace std;

// ctor
WtInputHandler::WtInputHandler(size_t pollms,vector<string>const&srclans,vector<string>const&trglans,const WEnvironment&env):WApplication(env){
  setTitle("xlate-web");       

  // main container
  WContainerWidget*pageContainer=new WContainerWidget();
  pageContainer->resize(400,WLength::Auto);

  // create language selection widget
  lanSelectionWidget_=new WtLanSelectionWidget(srclans,trglans,pageContainer);
  lanSelectionWidget_->selectionChanged().connect(this,&WtInputHandler::lanSelectionChanged);
  
  // create area for text
  WContainerWidget*textAreaContainer=new WContainerWidget(pageContainer);
  WBorderLayout*textAreaLayout=new WBorderLayout();
  textAreaContainer->setLayout(textAreaLayout);
  srcTextArea_=new WTextArea("");
  //srcTextArea_->keyWentUp().connect(this,&WtInputHandler::srcTextChanged);
  srcTextArea_->setReadOnly(false);
  trgTextArea_=new WTextArea("");
  trgTextArea_->setReadOnly(true);
  textAreaLayout->addWidget(srcTextArea_,WBorderLayout::West);
  textAreaLayout->addWidget(trgTextArea_,WBorderLayout::East);

  // create TRANSLATE button
  WContainerWidget*buttonContainer=new WContainerWidget(pageContainer);
  WBorderLayout*buttonLayout=new WBorderLayout();
  buttonContainer->setLayout(buttonLayout);
  translateButton_=new WPushButton("TRANSLATE");
  translateButton_->clicked().connect(this,&WtInputHandler::translatePushed);
  buttonLayout->addWidget(translateButton_,WBorderLayout::Center);

  // create timer
  timer_=new WTimer();
  timer_->setInterval(pollms);
  timer_->timeout().connect(this,&WtInputHandler::timerPopped);

  // add container to root
  root()->addWidget(pageContainer);

  // enable/disable various widgets
  processStateChange();
}
// language selection changed
void WtInputHandler::lanSelectionChanged(){
  processStateChange();
}
// translate button pushed
void WtInputHandler::translatePushed(){
  // start translation
  WString const&srctxt=srcTextArea_->text();
  translating_=true;
  processStateChange();
  startTimer();
}
// source text changed
/*
void WtInputHandler::srcTextChanged(){
}
*/
// poll timer popped
void WtInputHandler::timerPopped(){
// NOTE!
string sessid=WApplication::instance()->sessionId();
cout<<"==== sessid: "<<sessid<<endl;

  // NOTE! Must check for translated text
  trgTextArea_->setText(srcTextArea_->text());

  // enable for translation again
  stopTimer();
  translating_=false;
  processStateChange();
}
// start timer
void WtInputHandler::startTimer(){
  timer_->start();
}
// stop timer
void WtInputHandler::stopTimer(){
  timer_->stop();
}
// process change of state in widgets
void WtInputHandler::processStateChange(){
  // check if we are translating
  if(translating_){
    translateButton_->disable();
    lanSelectionWidget_->disable();
    trgTextArea_->setText("");
  }else{
    // make sure language selection is enabled
    lanSelectionWidget_->enable();

    // check if we have a valid language pair
    string srclan=lanSelectionWidget_->srclan();
    string trglan=lanSelectionWidget_->trglan();

    // enable/disable TRANSLATE button
    if(srclan!=trglan)translateButton_->enable();
    else translateButton_->disable();
  }
}
