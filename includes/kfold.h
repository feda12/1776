#ifndef KFOLD_H
#define KFOLD_H
#include <vector>
#include "mystrcat.h"
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <boost/foreach.hpp>
 
using namespace std;
 
#define foreach BOOST_FOREACH
 
template<class In>
class Kfold {
public:
    Kfold(int k, In _beg, In _end);
    template<class Out>
    void getFold(int foldNo, Out training, Out testing);
 
private:
    In beg;
    In end;
    int K; //how many folds in this
    vector<int> whichFoldToGo;
 
};
 
template<class In>
Kfold<In>::Kfold(int _k, In _beg, In _end) : beg(_beg), end(_end), K(_k) {
	if (K <= 0)
		throw runtime_error(Mystrcat("The supplied value of K is =", K,". One cannot create ", K, "no of folds"));

	//create the vector of integers
	int foldNo = 0;
	for (In i = beg; i != end; i++) {
		whichFoldToGo.push_back(++foldNo);
		if (foldNo == K)
			foldNo = 0;
	}
	if (!K)
	throw runtime_error(Mystrcat("With this value of k (=", K ,")Equal division of the data is not possible"));
	random_shuffle(whichFoldToGo.begin(), whichFoldToGo.end());
}

template<class In>
template<class Out>
void Kfold<In>::getFold(int foldNo, Out training, Out testing) {
	int k = 0;
	In i = beg;
	while (i != end) {
		if (whichFoldToGo[k++] == foldNo) {
			*testing++ = *i++;
		} else {
			*training++ = *i++;
		}
	}
}

#endif
