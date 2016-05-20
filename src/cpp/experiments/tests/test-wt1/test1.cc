// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015,2016. All rights reserved.

#include <iostream>
#include <boost/bind.hpp>
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

using namespace Wt;
using namespace std;

// HelloApplication class.
class HelloApplication:public WApplication{
public:
  HelloApplication(const WEnvironment&env);
private:
  WLineEdit *nameEdit_;
  WText *greeting_;
  WText *debug_text_;
  void debug(string const&info){debug_text_->setText(info);}
  void greet();
  void mouseMoved(WMouseEvent me){
    cerr<<"=== MOUSE MOVED ("<<me.window().x<<","<<me.window().y<<")"<<endl;
  }
};

// ctor - HelloApplication.
HelloApplication::HelloApplication(const WEnvironment&env):WApplication(env){
  setTitle("Hello world");       
  root()->addWidget(new WText("Your name, please ? "));
  nameEdit_=new WLineEdit(root());                    
  nameEdit_->setFocus();                             
  WValidator*val(new WValidator());
  val->setMandatory(true);
  nameEdit_->setValidator(val);
  WPushButton *button= new WPushButton("Greet me.", root());             
  button->setMargin(5, Left);                     
  root()->addWidget(new WBreak());               
  greeting_ = new WText(root());                
  root()->addWidget(new WBreak());               
  debug_text_=new WText(root());
  button->clicked().connect(this,&HelloApplication::greet);

  button->mouseMoved().connect(this,&HelloApplication::mouseMoved);

  nameEdit_->blurred().connect(boost::bind(&HelloApplication::debug,this,"NAME-EDIT BLURRED"));
  nameEdit_->focussed().connect(boost::bind(&HelloApplication::debug,this,"NAME-EDIT FOCUSED"));
  nameEdit_->enterPressed().connect(boost::bind(&HelloApplication::greet,this));
}
void HelloApplication::greet(){
    nameEdit_->setText("");
    greeting_->setText("Hello there, "+nameEdit_->text());
  }

// create app.
WApplication *createApplication(const WEnvironment& env){
cerr<<">>> IN createApplication"<<endl;
  return new HelloApplication(env);
}

// main.
int main(int argc,char**argv){
return WRun(argc,argv,&createApplication);
}


