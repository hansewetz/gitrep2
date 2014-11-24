// (C) Copyright Hans Ewetz 2010,2011,2012,2013,2014. All rights reserved.

#include <iostream>
#include <memory>
#include <functional>
#include "CLucene.h"
using namespace std;

using namespace lucene::analysis;
using namespace lucene::index;
using namespace lucene::document;
using namespace lucene::queryParser;
using namespace lucene::search;
using namespace lucene::store;

const TCHAR* docs[] = {
_T("Hello world"),
_T("Good by miserable world"),
_T("Foo Bar Tar"),
NULL
};
const TCHAR* queries[] = {
_T("hello World"),
_T("Good"),
_T("Foo Bar Good"),
_T("world Good"),
NULL
};

int main(){
  //WhitespaceAnalyzer analyzer;
  SimpleAnalyzer analyzer;
  try {
    //  ----------- build in memory index ---------------
    unique_ptr<Directory,std::function<void(Directory*)>>dir(FSDirectory::getDirectory("./index"),[](Directory*d){d->close();delete d;});
    //unique_ptr<Directory,std::function<void(Directory*)>>dir(new RAMDirectory,[](Directory*d){d->close();delete d;});
    {
      unique_ptr<IndexWriter,std::function<void(IndexWriter*)>>iwriter(new IndexWriter(dir.get(), &analyzer, true),[](IndexWriter*w){w->close();delete w;});
      Document doc;
      for (int j = 0; docs[j] != NULL; ++j) {
        doc.add( *_CLNEW Field(_T("contents"), docs[j], Field::STORE_YES | Field::INDEX_TOKENIZED) ); 
        iwriter->addDocument(&doc); doc.clear();
      }
    }
    //  ----------- do some searches ---------------
    unique_ptr<IndexReader,std::function<void(IndexReader*)>>ireader(IndexReader::open(dir.get()),[](IndexReader*r){r->close();delete r;});
    IndexSearcher searcher_(ireader.get());
    unique_ptr<IndexSearcher,std::function<void(IndexSearcher*)>>searcher(&searcher_,[](IndexSearcher*i){i->close();});
	
    QueryParser parser(_T("contents"), &analyzer);
    parser.setPhraseSlop(4);

    unique_ptr<Hits>hits(static_cast<Hits*>(0));
    for (int j = 0; queries[j] != NULL; ++j) {
      unique_ptr<Query>query(parser.parse(queries[j]));
      const wchar_t* qryInfo = query->toString(_T("contents"));
      printf("---> Query: %ls\n", qryInfo);
      delete[] qryInfo;
      hits.reset(searcher->search(query.get()));
      //cerr<<"Total #results: "<<hits->length()<<endl;
      for (size_t i=0; i < hits->length() && i<10; i++) {
        Document* d = &hits->doc(i);
        printf("[%d-score: %f] %ls\n", i, d->get(_T("contents")), hits->score(i));
      }
    }
  } catch (CLuceneError& e) {
    printf(" caught a exception: %s\n", e.twhat());
  } catch (...){
    printf(" caught an unknown exception\n");
  }
}

