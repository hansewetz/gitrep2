#include "soapxlateProxy.h"
#include "xlate.nsmap"
#include <boost/program_options.hpp>
using namespace std;
namespace po=boost::program_options;

// default url and port to connect to
string url{"http://localhost"};
int port=6060;

// program options and usage
namespace{
  po::options_description desc{string("usage: -p <port> -u <url> -n <num> -h")};
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
      ("port,p",po::value<int>(),"server port (optional, default 6060)")
      ("url,u",po::value<string>(),"server port (optional, default: http://localhost)")
      ("val,v",po::value<double>(),"value to translate (mandatory)");
    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);
    po::notify(vm);

    // check for help
    if(vm.count("help"))usage();

    // get port
    if(vm.count("port")){
      if((port=vm["port"].as<int>())<=0)usage();
    }
    // get url (construct default one first - if specific construct specified one)
    if(vm.count("url"))url=vm["url"].as<string>();

    // get value to translate
    if(!vm.count("val"))usage();
    auto val=vm["val"].as<double>();
 
    // call service
    ns__asString res;
    xlateProxy xlate;
    string endpoint{url+":"+boost::lexical_cast<string>(port)};
    xlate.soap_endpoint=endpoint.c_str();
    xlate.translate(val,res);

    // check for errors
    if (xlate.error)xlate.soap_stream_fault(std::cerr);
    else cout<<res.str<<endl;
  } 
  catch(std::exception const&e){
    cerr<<"exception: "<<e.what()<<endl;
    exit(1);
  }
  return 0;
}
