#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <boost/lexical_cast.hpp>
using namespace std;

// This array contains a list of names that have been reported at 
// the same physical address.
string names[] =
{
	"John Doe",
	"Nancy Doe",
	"Mr. John E. Doe",
	"Nancy Wilson Doe",
	"John Doe",
	"Jonathon Doe",
	"Mrs. Nancy Elizabeth Doe",
	"Nancy Elizabeth Doe",
	"Peter Doe"
};
const int COUNT=sizeof(names)/sizeof(names[0]);

// This matrix contains the results of comparing each of the 
// names above with every other name in the list.  The value at 
// scores[m][n] is the similarity score produced by comparing the 
// m'th name with the n'th name.  The similarity threshold was 
// set at 90%, so any score greater than zero should be 
// considered to be a probable match.
// Values are set to -1 for comparisons of a name against itself,
// as this would not be considered a cluster.
int scores[COUNT][COUNT]={
	{  -1,   0,  99,   0, 100,  97,   0,   0,   0 },
	{   0,  -1,   0,  97,   0,   0,  99,  99,   0 },
	{  99,   0,  -1,   0,  97,  92,   0,   0,   0 },
	{   0,  97,   0,  -1,   0,   0,  96,  97,   0 },
	{ 100,   0,  97,   0,  -1,  91,   0,   0,   0 },
	{  97,   0,  92,   0,  91,  -1,   0,   0,   0 },
	{   0,  99,   0,  96,   0,   0,  -1,  99,   0 },
	{   0,  99,   0,  97,   0,   0,  99,  -1,   0 },
	{   0,   0,   0,   0,   0,   0,   0,   0,  -1 }
};

/* ---------------------- A FEW COMMENTS -----------------------
  - the code is written by wrapping up clusters and cluster elements as objects. It could also have been
    coded in a (much) more compact way using simple data structures such as tuples - but would become a little less readable/maintaneable
    Which way that is best depends on if this is a one time shot or if it should be used by some other code.
    (maybe it's overkill to use classes in this problem ...?)
  - if the code should be used as an api, it should be split into headers and source files
  - It's not completly clear to me if the name which is the 'center' of a cluster should be first or last in the cluster when printed
    Right now, I've put it at the end of the cluster.
  - my understanding is that if the 'score' is >= 90 for a pair of names, they belong to the same cluster
  - depending on the environment in which the clusters are used, the classes may need a few more method
  - The way this should be coded depends also on if the code should be scaleable to very large data sets (million, 100 millions ...)
  - right now the code building clusters rely on the order in which 'scores' are parsed. This could be made more generic at the cost of exection time
  - I have assumed the score matrix is symmetric - i.e., score(name,name2)==score(name2,name1)
 ------------------------------------------------------------------*/

// structure representing cluster element
class ClusterElement{
public:
  // print operator
  friend ostream&operator<<(ostream&os,ClusterElement const&c){
    return os<<"["<<c.score_<<"|"<<c.name_<<"]";
  }
  // ctors,assign,dtor
  ClusterElement(int ind,string const&name,int score):ind_(ind),name_(name),score_(score){}
  ClusterElement(ClusterElement const&)=default;
  ClusterElement(ClusterElement&&)=default;
  ClusterElement&operator=(ClusterElement const&)=default;
  ClusterElement&operator=(ClusterElement&&)=default;
  ~ClusterElement()=default;

  // get info from element
  int ind()const{return ind_;}
  string const&name()const{return name_;}
  int score()const{return score_;}
private:
  int ind_;  // questionable if we should keep index from array where name came from ...
  string name_;
  int score_;
};
// structure representing cluster
class Cluster{
// only let function building clusters be able to add elements to cluster)
// (not so elegant ... but guarentees that clusters are built correctly)
// (maybe should be static method ... ?)
friend vector<Cluster>buildClusters(int,string*,int*,int);
public:
  // print operator
  friend ostream&operator<<(ostream&os,Cluster const&c){
    for(auto const&el:c.elements_)os<<*el;
    return os;
  }
  // ctors,assign,dtor
  Cluster()=default;
  Cluster(Cluster const&)=default;
  Cluster(Cluster&&)=default;
  Cluster&operator=(Cluster const&)=default;
  Cluster&operator=(Cluster&&)=default;
  ~Cluster()=default;

  // get signature of cluster
  string const&signature()const{return signature_;}

  // get #elements in cluster
  int size()const{return elements_.size();}
private:
  // add to cluster (must add in order of index)
  // (private since adding elements must be done in index order)
  void add(int ind,string const&name,int score){
    elements_.push_back(make_shared<ClusterElement>(ind,name,score));
    signature_+=boost::lexical_cast<string>(ind)+"-";
  }
  // sort elements by score in desc. order
  void sortElementsByScoreDesc(){
    sort(elements_.begin(),elements_.end(),[](shared_ptr<ClusterElement>e1,shared_ptr<ClusterElement>e2){return e1->score()>e2->score();});
  }
  // state of objct
  string signature_;
  vector<shared_ptr<ClusterElement>>elements_;
};
// build clusters
// (make it a little more eableortable by not using globals)
vector<Cluster>buildClusters(int count,string*names,int*scores,int threshold){
  // return value
  vector<Cluster>clusters(count);

  // build clusters 
  // (we might create clusters which are identical - will remove them later)
  // (elements are added in order of increasing index into names[] ...)
  // (so that signatures of identical clusers will be the same)
  for(int i=0;i<count;++i){
    // add element to cluster 
    clusters[i].add(i,names[i],-1);

    // add names to clusters
    for(int j=i+1;j<count;++j){
      // check if names[row] belong to same cluster as names[col]
      int score{scores[i*count+j]};
      if(score>=threshold){
        // add names[i] and names[j] to the clusters they belong to
        clusters[i].add(j,names[j],score);
        clusters[j].add(i,names[i],score);
      }
    }
  }
  // remove single element clusters, sort based on signature of cluster and remove duplicate ones
  // (clusters with same elements have same signature since names are added in order of index into 'names')
  // (move semantics will avoid object copying in swap())
  clusters.erase(remove_if(clusters.begin(),clusters.end(),[](Cluster const&c){return c.size()==1;}),clusters.end());
  sort(clusters.begin(),clusters.end(),[](Cluster const&c1,Cluster const&c2){return c1.signature()<c2.signature();});
  clusters.erase(unique(clusters.begin(),clusters.end(),[](Cluster const&c1,Cluster const&c2){return c1.signature()==c2.signature();}),clusters.end());

  // sort elements within each cluster based on score
  for(auto&cluster:clusters)cluster.sortElementsByScoreDesc();

  // return clusters
  // (will use move semantics so no copying will be done)
  return clusters;
}
// The task is to write a program that will identify "clusters" 
// of similar names, so those names can be collapsed into single 
// name records.
// Each cluster should be identified and the names that appear in 
// each cluster should be ordered by descending similarity score.
// Names that do not appear in clusters should not be included in 
// the output.
int main(){
  // threshold for belonging to same cluster
  constexpr int threshold{90};

  // keep clusters in vector
  vector<Cluster>clusters{buildClusters(COUNT,names,scores[0],threshold)};

  // print clusters
  for(auto const&cluster:clusters)cout<<cluster<<endl;
}

