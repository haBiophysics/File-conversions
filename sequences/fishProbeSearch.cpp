#include <iostream>
#include <set>
#include <vector>

// Alternative idea: http://www.hicomb.org/papers/HICOMB2011-01.pdf

using namespace std;

const int Tm_threshold = 70; // oC, if melting temperature lower, then "binds"
const int k = 7; // Length of the oligo patterns to build a tree
const string database;
set <pair<string, vector<int> > > tree;


bool pairComp (const pair<string, vector<int> >& lhs, const pair<string, vector<int> >& rhs) {
	return lhs.first < rhs.first;
}


// Arranges all k-mers of a database and all start positions containing them.
void buildTree () {
	tree.clear();
	int dataBaseLength = database.size();
	int lastStartPos = dataBaseLength-k+1;
	vector <int> emptyVec;

	for (int i=0; i<lastStartPos; i++) {
		string word = database.substr(i,k);
		pair<string, vector<int> > temp (word, emptyVec);
		pair< set<pair<string, vector<int> > >::iterator, bool> ret = tree.insert(temp);

		if (ret.second == false) { // Not inserted
			set<pair<string, vector<int> > >::iterator it = ret.first;
			pair<string, vector<int> > element = *it;
			element.second.push_back(i);
			tree.erase(it);
			tree.insert(element);
		}
	}

	return;
}


double evaluateMeltingTemperature (const string &x, const string& y) {
	// Fill here somehow...
	return 0;
}


bool searchTree (const string& key, int ignorePosStart, int ignorePosEnd) {
	// Return to position of first hit
	for (int i=0; i<key.size(); i++) {
		pair<string,int> word (key.substr(i,k), 0);
		set<pair<string, vector<int> > >::iterator it = tree.find(word);

		if (it != tree.end()) {
			for (int j=0; j<it->second.size(); j++) {
				if (it->second.at(j) < ignorePosStart || it->second.at(j) > ignorePosEnd) {
					// A reasonably similar region found, make a further check either by melting temperature or dynamic programming
					string dbNeighbourhood = database.substr(i-key.size(), 2*key.size());
					int Tm = evaluateMeltingTemperature (dbNeighbourhood,key);
					if (Tm < Tm_threshold) // It binds to here with high affinity, usuitable
						return true;
				}
			}
		}
	}

	return false; // Else, not found
}


vector <bool> searchAllMembers (const vector<string>& oligos, const vector <int>& geneStartPos, const vector <int>& geneEndPos) {
	vector <bool> hitFlag (oligos.size());

	#pragma omp parallel for schedule (dynamic, 100)
	for (int i=0; i<oligos.size(); i++) {
		hitFlag.at(i) = searchTree (oligos.at(i), geneStartPos.at(i), geneEndPos.at(i) );
	}

	return hitFlag;
}
