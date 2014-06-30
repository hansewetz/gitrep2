#include <boost/program_options.hpp>
#include "StockQuoteSoapBinding.nsmap"
using namespace std;
namespace po=boost::program_options;

// program options and usage
namespace{
  po::options_description desc{string("usage: -n <num> -h")};
  void usage(){
    cerr<<desc;
    exit(1);
  }
}
// --- client
int main(int argc, char **argv){
  try{
    // parse command line
    desc.add_options()
      ("help,h","print help message")
      ("sym,s",po::value<string>(),"symbol of stock (mandatory)");
    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);
    po::notify(vm);

    // check for help
    if(vm.count("help"))usage();

    // get value to translate
    if(!vm.count("sym"))usage();
    string sym=vm["sym"].as<string>();

    // setup request and resp variables
    _ns2__TradePriceRequest req;
    req.tickerSymbol=strdup(sym.c_str()); // NOTE! Not sure how to handle memory here ...
    _ns2__TradePrice resp;

    // make request
    struct soap *soap = soap_new();
cout<<"Calling server ..."<<endl;
    int stat=soap_call___ns1__GetLastTradePrice(soap,NULL,NULL,&req,&resp);
cout<<"Calling server done ..."<<endl;
    
    // print result
    if (stat == SOAP_OK){
      cout<<sym<<": "<<resp.price<<endl;
    }
    else{ // an error occurred 
       soap_print_fault(soap, stderr); 
    }
  } 
  catch(std::exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
    exit(1);
  }
  return 0;
}
