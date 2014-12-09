#ifndef __NORWEGIAN_TMX_EXTRACTOR_H__
#define __NORWEGIAN_TMX_EXTRACTOR_H__
#include <string>
#include <vector>
#include <utility>
#include <iosfwd>
#include <boost/filesystem.hpp>
#include <libxml/xmlreader.h>
namespace mtdata{

// class for parsing a Norwegian TMX file
// (note: this class works with the tmx files provided by the Norwegian Ministry of Foreign Affairs)
// (this class should not be used with any other data)
// (the constructor reads the entire file and extracts relevant data - the data can later be queried using getter methods)
class NorwegianTmxExtractor{
public:
  class Segment{
  friend class NorwegianTmxExtractor;
  public:
    Segment(Segment const&)=default;
    Segment(Segment&&)=default;
    Segment&operator=(Segment const&)=default;
    Segment&operator=(Segment&&)=default;
    std::string const&srclan()const;
    std::string const&targetlan()const;
    std::string const&srcseg()const;
    std::string const&targetseg()const;
  private:
    // only used by NorwegianTmxExtractor class to build segments
    Segment()=default;
    void checkSeg()const;

    std::string srclan_;
    std::string targetlan_;
    std::string srcseg_;
    std::string targetseg_;
  };
  // print debug operator for Segment
  friend std::ostream&operator<<(std::ostream&os,Segment const&s);

  // state related variables
  enum states{NOT_STARTED=0,HEADER=1,TU=2,TUV=3,SEG=4};

  // ctors,assign,dtor
  // (make it non-movable an dnon copyable - no need to introduce complications)
  NorwegianTmxExtractor(boost::filesystem::path const&tmxpath);
  NorwegianTmxExtractor(NorwegianTmxExtractor const&)=delete;
  NorwegianTmxExtractor(NorwegianTmxExtractor&&)=delete;
  NorwegianTmxExtractor&operator=(NorwegianTmxExtractor const&)=delete;
  NorwegianTmxExtractor&operator=(NorwegianTmxExtractor&&)=delete;
  ~NorwegianTmxExtractor()=default;

  // getters
  std::string dbid()const;
  std::vector<NorwegianTmxExtractor::Segment>segs()const;
private:
  // helper function
  void processNode(xmlTextReaderPtr reader);

  // state related variables
  states state_=NorwegianTmxExtractor::NOT_STARTED;
  boost::filesystem::path tmxpath_;

  // some state variables used during extraction of data from tmx file
  bool getdbidFromHeader_{false};
  std::string currlan_{""};
  std::string currseg_{""};
  Segment seg_;

  // data extracted from tmx file
  std::string dbid_{""};
  std::vector<NorwegianTmxExtractor::Segment>segs_;
};
}
#endif
