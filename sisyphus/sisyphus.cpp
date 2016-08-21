// Tunc Kayikcioglu, 2016
// Ha lab, Johns Hopkins University

// Example of how to manipulate sequencing reads.
// Checks a large oligo library against nons-specific binding in the target genome.
// Note: only exact matches are considered.
// To improve computational efficiency, fragments to be searched are organized in a tree-structure. Be aware that this serves an assessment of experimental, but does not follow a state of the art approach.
// Implemented by STL::set against which the search is performed.


#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <set>
#include <omp.h>

using namespace std;
ofstream logfile;


string complementary (const string& x) {
	string out;
	for (int i=x.size()-1; i>=0; i--) {
		char temp;
		switch (x.at(i)) {
			case 'A':
				temp = 'T';
				break;
			case 'T':
				temp = 'A';
				break;
			case 'G':
				temp = 'C';
				break;
			case 'C':
				temp = 'G';
				break;
			default:
				throw("Unexpected base in a DNA molecule.\n");
		}
		out += temp;
	}
	return out;
}


// Takes complementary strand by checking a 2-bit representation of the bases
// 11 -> 00
// 01 -> 10
// 00 -> 11
// 10 -> 01
vector <bool> complementary (const vector<bool>& x) {
	int length = x.size();
	if (length%2 != 0) {
		cout << "ERROR: incomplete DNA sequence passed" << endl;
		throw;
	}
	
	vector <bool> out;
	out.resize(length);
	for (int i=0; i<length; i+=2) {
		out[i] = !x[length-2-i];
		out[i+1] = !x[length-1-i];
	}
	return out;
}


// Converts base sequence stored in the 2-bit vector form into standard ATCG characters.
string bool2seq (vector <bool> in) {
	int length = in.size();
	if (length%2 != 0) {
		cout << "ERROR: Incomplete DNA sequence passed" << endl;
		throw;
	}
	
	string out(length/2, 'N');
	for (int i=0; i<length/2; i++) {
		if (in[2*i] == 0) {
			if (in[2*i+1] == 0)
				out[i] = 'A';
			else
				out[i] = 'G';
		}
		else {
			if (in[2*i+1] == 0)
				out[i] = 'C';
			else
				out[i] = 'T';
		}
	}
	return out;
}


// Converts base sequence stored in the form of ATCG characters into 2-bit vectors.
// Due to the operation principle of boolean vectors, parallelization of this leads to data race.
vector <bool> seq2bool (string in) {
	int length = in.size();
	vector <bool> out (2*length);
	
	for (int i=0; i<length; i++) {
		switch(in[i]){
			case 'A':
			case 'a':
				out[2*i] = 0;
				out[2*i+1] = 0;
				break;
			case 't':
			case 'T':
				out[2*i] = 1;
				out[2*i+1] = 1;
				break;
			case 'g':
			case 'G':
				out[2*i] = 0;
				out[2*i+1] = 1;
				break;
			case 'c':
			case 'C':
				out[2*i] = 1;
				out[2*i+1] = 0;
				break;
			default:
				cout << "Unexpected base in a DNA molecule." << endl;
				throw;
		}
	}
	
	return out;
}


// Reads genome from input file (in FASTA format)
vector <string> readGenome (const string filename) {
	fstream infile;
	infile.open(filename.c_str());
	if (infile.is_open() == false) {
		cout << "Input file " << filename << " not found." << endl;
		throw;
	}
	
	cout << "Reading genome file " << filename << "..." << endl;
	string genome;
	while (true) {
		string line;
		if (infile.eof() == true)
			break;
		std::getline(infile, line);
		
		if (line[0] != '>')
			genome += line;
	}
	infile.close();
	genome += 'E';
	
	
	// Go through the genome, convert everything to uppercase.
	// If multiple consecutive non-standard bases (ACTG), put one N only.
	cout << "Reorganizing genome..." << endl;
	vector <string> out;
	string stretch;
	stretch.reserve(1e8);
	for (int i=0; i<genome.size(); i++) {
		char base = genome.at(i);
		switch(base){
			case 'A':
			case 'a':
				stretch.push_back('A');
				break;
			case 't':
			case 'T':
				stretch.push_back('T');
				break;
			case 'g':
			case 'G':
				stretch.push_back('G');
				break;
			case 'c':
			case 'C':
				stretch.push_back('C');
				break;
			default: // Break flag encountered.
				if (stretch.size() > 10) // If very short stretch between undetermined sequences, disregard.
					out.push_back(stretch);
				stretch.clear();
				stretch.reserve(1e8);
				break;
		}
	}
	
	return out;
}


// Imports a library from a file each row of which contain one library element.
// Unique elements only, repeats detected and discarded.
set <string> readProbeList (const string filename) {
	fstream infile;
	infile.open(filename.c_str());
	
	set <string> list;
	int identicals = 0;
	while (true) {
		string line;
		infile >> line;
		if (infile.eof() == true)
			break;
		if (line.size() == 0 || line.at(0) == '>')
			continue;
		
		int prevsize = list.size(); 
		list.insert(line);
		if (list.size() == prevsize) { // Redundant element found
			logfile << "ERROR: Identical element: " << line << endl;
			identicals++;
		}
	
		if (list.size()%1000 == 0) {
			cout << "\r" << list.size() << " fragments read";
			fflush(stdout);
		}
	}
	cout << endl;
	cout << identicals << " repeating elements found in the library." << endl;
	logfile << identicals << " repeating elements found in the library." << endl;
	logfile << endl;
	
	infile.close();
	return list;
}


// Creates sub-sequences of a given length tiling each element in the library
set <string> buildFragments (const set <string>& library, const int length) {
	set <string> list;
	for (set<string>::iterator it=library.begin(); it!=library.end(); ++it) {
		string element = *it;
		for (int j=0; j<=element.size()-length; j++) {
			string subelement = element.substr(j, length);
			list.insert(subelement);
			list.insert(complementary(subelement)); // Also include complementary sequences into the search space
		}
	}
	
	return list;
}


// Creates sub-sequences of a given length tiling each element in the library
set <vector <bool> > buildFragments_2bit (const set <string>& library, const int length) {
	set <vector <bool> > list;
	for (set<string>::iterator it=library.begin(); it!=library.end(); ++it) {
		string element = *it;
		for (int j=0; j<=element.size()-length; j++) {
			vector<bool> subelement = seq2bool(element.substr(j, length));
			list.insert(subelement);
			list.insert(complementary(subelement)); // Also include complementary sequences into the search space
		}
	}
	
	return list;
}



set <string> searchGenome (const vector <string>& genome, const set <string>& list) {
	set <string> hits;
	const int querySize = (*list.begin()).size();
	
	for (int j=0; j<genome.size(); j++) {
		int targetLimit = genome.at(j).size()-querySize+1;
	
		// Search the genome fragment for hits and record if found some.
		# pragma omp parallel for
		for (int i=0; i<targetLimit; i++) {
			string genomeElement = genome[j].substr(i,querySize);
			if(list.find(genomeElement) != list.end()) { // Check against all elements in the library
				// Hit found, record this.
				#pragma omp critical
				{
					hits.insert(genomeElement);
				}
			}
		}
	}
	
	// Remember that the reported hits may be against the complementary sequences as well.	
	return hits;
}


set <string> searchGenome_2bit (const vector <string>& genome, const set <vector <bool> >& list) {
	set <string> hits;
	const int querySize = (*list.begin()).size()/2;
	
	for (int j=0; j<genome.size(); j++) {
		int targetLimit = genome.at(j).size()-querySize+1;
	
		// Search the continuous genome fragment for hits and record if found some.
		# pragma omp parallel for
		for (int i=0; i<targetLimit; i++) {
			vector <bool> genomeElement = seq2bool(genome[j].substr(i,querySize)); // retrieves a sub-sequence from the genome in 2-bit format
			if (list.find(genomeElement) != list.end()) { // check against all elements in the library
			// Hit found, record this.
				#pragma omp critical
				{
					hits.insert(bool2seq(genomeElement));
				}
			}
		}
	}
	
	return hits;
}



// Goes through the whole list of hits and identifies which construct(s) in your library it impacts. A report file is generated containing a list of all such problems.
void analyzeHits (const vector <string>& hits, const vector <string>& list) {
	if (hits.size() == 0) {
		cout << "Your library contains no hits against the provided genome." << endl;
		logfile << "Your library contains no hits against the provided genome." << endl;
		return;
	}
	
	int fragmentLength = hits.at(0).size();
	int hitCount = 0;
	for (int i=0; i<hits.size(); i++) {
		string hit = hits.at(i);
		string hitc = complementary(hit);
		bool hitfound = false;
		for (int j=0; j<list.size(); j++) {
			string element = list[j];
			for (int k=0; k<(element.size()-fragmentLength+1); k++) {
				string subelement = element.substr(k, fragmentLength);
				if (subelement == hit || subelement == hitc) {
					if (hitfound == false) {
						logfile << "HIT (" << ++hitCount << "): " << endl;
						logfile << hit << endl;
						hitfound = true;
					}
					logfile << element << endl;
					break;
				}
			}
		}
		if (hitfound == true)
			logfile << endl;
	}
	
	logfile << "There were a total of " << hitCount << " hits in your library." << endl;
	cout << "There were a total of " << hitCount << " hits in your library." << endl;
	return;
}


template <class T1, class T2>
void analyzeHits (const set <T1>& hits, const vector <T2>& list) {
	vector <T1> temp (hits.begin(), hits.end());
	return analyzeHits(temp,list);
}


template <class T1, class T2>
void analyzeHits (const vector <T1>& hits, const set <T2>& list) {
	vector <T2> temp (list.begin(), list.end());
	return analyzeHits(hits,temp);
}


template <class T1, class T2>
void analyzeHits (const set <T1>& hits, const set <T2>& list) {
	vector <T1> temp1 (hits.begin(), hits.end());
	vector <T2> temp2 (list.begin(), list.end());
	return analyzeHits(temp1,temp2);
}


void analyzeHits (const set <vector <bool> >& hits, const set <string>& list) {
	set <string> temp;
	for (set <vector <bool> >::iterator it=hits.begin(); it!=hits.end(); it++)
		temp.insert(bool2seq(*it));
		
	return analyzeHits(temp, list);
}


// DNA sequences are considered as strings of 8-bit characters, i.e. ATCG as usual.
void run_sequencesAsStrings (void) {
	logfile.open("./output.log", std::ofstream::out);
	double begin = omp_get_wtime();
	
	cout << "Importing the genome..." << endl;
	vector <string> genome = readGenome ("./genome.fa");
	
	cout << "Reading the library..." << endl;
	set <string> list = readProbeList ("./probes.txt");
	
	cout << "Generating search fragments..." << endl;
	set <string> fragments = buildFragments(list ,20);
	cout << fragments.size() << " fragments in total" << endl;
	
	cout << "Searching for fragments in the genome..." << endl;
	set <string> hits = searchGenome(genome, fragments);
	
	cout << "Generating output..." << endl;
	analyzeHits(hits, list);
	
	double end = omp_get_wtime();
	int duration = (end-begin);
	cout << "Total time elapsed: " << duration << " seconds (approx. " << (duration+30)/60 << " min)" << endl;
	
	logfile.close();
	cout << "Done!" << endl;
	return;
}



// This implementation stores bases as 2-bits, hoping to decrease the required memory (of concern only if dealing with very large libraries). Computational time is slightly longer due to conversions, but may pay itself off as size increases.
void run_sequencesAs2bitVectors (void) {
	logfile.open("./output.log", std::ofstream::out);
	double begin = omp_get_wtime();
	
	cout << "Importing the genome..." << endl;
	vector <string> genome = readGenome ("./genome.fa");
	
	cout << "Reading the library..." << endl;
	set <string> list = readProbeList ("./probes.txt");
	
	cout << "Generating search fragments..." << endl;
	set <vector <bool> > fragments = buildFragments_2bit (list, 20);
	cout << fragments.size() << " fragments in total" << endl;
	
	cout << "Searching for fragments in the genome..." << endl;
	set <string> hits = searchGenome_2bit(genome, fragments);
	
	cout << "Generating output..." << endl;
	analyzeHits(hits, list);
	
	double end = omp_get_wtime();
	int duration = (end-begin);
	cout << "Total time elapsed: " << duration << " seconds (approx. " << (duration+30)/60 << " min)" << endl;
	
	logfile.close();
	cout << "Done!" << endl;
	return;
}



int main (void) {
	cout << "Standard run where bases are represented as ACTG." << endl;
	run_sequencesAsStrings ();
	
	cout << "Now trying to represent bases as 2bits, rather than 8bits." << endl;
	run_sequencesAs2bitVectors();
	return 0;
}



