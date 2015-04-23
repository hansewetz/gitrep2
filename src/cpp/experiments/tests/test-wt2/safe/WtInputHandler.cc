// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.
#include "WtInputHandler.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WText>
#include <Wt/WComboBox>
#include <Wt/WHBoxLayout>
#include <Wt/WVBoxLayout>
#include <Wt/WPushButton>
#include <Wt/WFileUpload>
#pragma GCC diagnostic pop
#include <iostream>
#include <string>
#include <vector>
using namespace Wt;
using namespace std;

namespace{
// NOTE! Should be retrieved from available engines
vector<string>lans{"en","nb","sv","el"};
}

// ctor
WtInputHandler::WtInputHandler(const WEnvironment&env):WApplication(env){
  setTitle("xlate-web");       

  // main containers
  WContainerWidget*pageContainer=new WContainerWidget();
  pageContainer->resize(200,WLength::Auto);
  WContainerWidget*lanSelectionContainer=new WContainerWidget();
  WHBoxLayout*lanSelection=new WHBoxLayout();
  lanSelectionContainer->setLayout(lanSelection);
  pageContainer->addWidget(lanSelectionContainer);
  
  // setup source selection
  WContainerWidget*srcContainer=new WContainerWidget();
  WVBoxLayout*srcbox=new WVBoxLayout();
  srcContainer->setLayout(srcbox);
  srcbox->addWidget(new WText("source"));
  srcList_=new WComboBox();
  for(auto const&lan:lans)srcList_->addItem(lan);
  srcList_->setCurrentIndex(0);
  srcList_->changed().connect(this, &WtInputHandler::srcListChanged);
  srcbox->addWidget(srcList_);
  lanSelection->addWidget(srcContainer);

  // setup target selection
  WContainerWidget*trgContainer=new WContainerWidget();
  WVBoxLayout*trgbox=new WVBoxLayout();
  trgContainer->setLayout(trgbox);
  trgbox->addWidget(new WText("target"));
  trgList_=new WComboBox();
  for(auto const&lan:lans)trgList_->addItem(lan);
  trgList_->setCurrentIndex(0);
  trgList_->changed().connect(this, &WtInputHandler::trgListChanged);
  trgbox->addWidget(trgList_);
  lanSelection->addWidget(trgContainer);

  // add file uploader
  fileUploader_=new WFileUpload();
  fileUploader_->setMultiple(false);
  fileUploader_->setFileTextSize(40);
  fileUploader_->changed().connect(fileUploader_,&WFileUpload::upload);
  fileUploader_->uploaded().connect(this,&WtInputHandler::uploaded);
  fileUploader_->fileTooLarge().connect(this,&WtInputHandler::uploadFileTooLarge);
// NOTE!
  pageContainer->addWidget(fileUploader_);

  // setup TRANSLATE button
  translateButton_=new WPushButton("TRANSLATE");
  translateButton_->clicked().connect(this,&WtInputHandler::translatePushed);
  pageContainer->addWidget(translateButton_);

  // add container to root
  root()->addWidget(pageContainer);

  // enable/disable various widgets
  enableHelper();
}
// source language changed
void WtInputHandler::srcListChanged(){
  enableHelper();
}
// target language changed
void WtInputHandler::trgListChanged(){
  enableHelper();
}
// check if we have a valid language pair
void WtInputHandler::enableHelper(){
  // check if we have a valid language pair
  string srclan=srcList_->currentText().narrow();
  string trglan=trgList_->currentText().narrow();
  validLanpair_=srclan!=trglan;

  // enable/disable TRANSLATE button
  if(validLanpair_&&haveFiles_)translateButton_->enable();
  else translateButton_->disable();
}
// translate button pushed
void WtInputHandler::translatePushed(){
  cout<<"==== TRANSLATE ..."<<endl;
}
// a file was uploaded
void WtInputHandler::uploaded(){
  vector<Http::UploadedFile>files=fileUploader_->uploadedFiles();
  for(size_t i=0;i<files.size();++i){
    // files[i].stealSpoolFile();
    cout<<"=== upload, filename: "<<files[i].clientFileName()<<", contentType: "<<files[i].contentType()<<", spoolName: "<<files[i].spoolFileName()<<endl;
    haveFiles_=true;
  }
  // make sure widgets are in correct state
  enableHelper();
}
void WtInputHandler::uploadFileTooLarge(){
  // NOTE!
  cout<<"=== FILE TOO LARGE"<<endl;
}
