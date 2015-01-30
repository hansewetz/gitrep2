// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014,2015. All rights reserved.

#include <iostream>

#include <boost/shared_ptr.hpp>

#include <ql/time/daycounters/actual360.hpp>
#include <ql/time/date.hpp>
#include <ql/handle.hpp>
#include <ql/quotes/simplequote.hpp>
#include <ql/termstructures/yieldtermstructure.hpp>
#include <ql/termstructures/yield/flatforward.hpp>
#include <ql/instruments/bonds/zerocouponbond.hpp>
#include <ql/indexes/ibor/usdlibor.hpp>
#include <ql/pricingengines/bond/discountingbondengine.hpp>

using namespace std;
using namespace QuantLib;

// create flat-rate yield structure
boost::shared_ptr<YieldTermStructure>flatRate(const Date&today,Rate forwardRate,const DayCounter&dc) {
  boost::shared_ptr<Quote>q=boost::shared_ptr<Quote>(new SimpleQuote(forwardRate));
  return  boost::shared_ptr<YieldTermStructure>(new FlatForward(today,Handle<Quote>(q),dc));
}

// main.
int main(){
  // dates
  Date today(30,November,2006);
  Settings::instance().evaluationDate()=today; // some singleton - thread safe ???

  // discount curve (constant contineous rate)
  boost::shared_ptr<YieldTermStructure>yieldStruct{flatRate(today,0.03,Actual360())};
  Handle<YieldTermStructure>discountCurve(flatRate(today,0.03,Actual360()));

  // pricing engine for (constant contineous rate)
  boost::shared_ptr<PricingEngine>bondEngine(new DiscountingBondEngine(discountCurve));

  // zero coupon bond instrument
  Natural settlementDays = 1;
  Real faceAmount{1000000.0}; // ??? what is this ... no impact ... ???
  ZeroCouponBond bond1(settlementDays,           // days for settlement
                       UnitedStates(UnitedStates::GovernmentBond), // calender
                       faceAmount,               // ...
                       Date(30,November,2008),   // maturity date
                       ModifiedFollowing,100.0,  // business day convention
                       Date(30,November,2004));  // issue date

  // add pricing engine to bond
  bond1.setPricingEngine(bondEngine);

  // get clean price
  Real price=bond1.cleanPrice();
  cout<<"clean price: "<<price<<endl;
}

