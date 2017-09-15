#include <iostream>
#include <string>
#include <algorithm>

using namespace std;


void countBases (string s) {
	int Acount = 0;
	int Tcount = 0;
	int Ccount = 0;
	int Gcount = 0;
	
	for (int i=0; i<s.size(); i++) {
		switch ( s.at(i) ) {
			case 'A':
			case 'a':
				Acount++;
				break;
			case 'T':
			case 't':
				Tcount++;
				break;
			case 'C':
			case 'c':
				Ccount++;
				break;
			case 'G':
			case 'g':
				Gcount++;
				break;
			default:
				cout << "ERROR: Unknown base in DNA sequence" << endl;
				throw(1);
		}
	}
	
	cout << Acount << "\t" << Ccount << "\t" << Tcount << "\t" << Gcount << endl;
	return;
}



string translate (string& DNA) {
	string RNA(DNA.size(), 'N');
	
	for (int i=0; i<DNA.size(); i++) {
		switch ( DNA.at(i) ) {
			case 'A':
			case 'a':
				RNA.at(i) = 'A';
				break;
			case 'T':
			case 't':
				RNA.at(i) = 'U';
				break;
			case 'C':
			case 'c':
				RNA.at(i) = 'C';
				break;
			case 'G':
			case 'g':
				RNA.at(i) = 'G';
				break;
			default:
				cout << "ERROR: Unknown base in DNA sequence" << endl;
				throw(1);
		}
	}
	
	return RNA;
}



// Generates the reverse-complementary of a given strand
string reverseComplement (const string& s) {
	string out (s.size(), 'N');
	
	for (int i=0; i<s.size(); i++) {	
		char temp;
		switch (s.at(s.size()-1-i)) {
			case 'A':
			case 'a':
				temp = 'T';
				break;
			case 'T':
			case 't':
				temp = 'A';
				break;
			case 'G':
			case 'g':
				temp = 'C';
				break;
			case 'C':
			case 'c':
				temp = 'G';
				break;
			default:
				cout << "Unexpected base in a DNA molecule.\n" << endl;
				throw(1);
		}
		out.at(i) = temp;
	}
	
	return out;
}



// Counts the basewise difference between two sequences. Lower scores are better.
int HammingDistance (const string& a, const string& b) {
	int size = a.size();
	if (b.size() != size) {
		cout << "Error: string lengths must match." << endl;
		throw(1);
	}
		
	int dist = 0;
	for (int i=0; i<size; i++) {
		if (a.at(i) != b.at(i))
			dist++;
	}
	
	return dist;
}



string translation (string RNA) {
	transform(RNA.begin(), RNA.end(), RNA.begin(), ::toupper);
	string protein;
	
	for (int i=0; i<RNA.size(); i+=3) {	
		string codon = RNA.substr(i,3);
		
		if (codon == "AUU" || codon == "AUC" || codon == "AUA")
			protein.push_back('I');
		else if (codon == "CUU" || codon == "CUC" || codon == "CUA" || codon == "CUG" || codon == "UUA" || codon == "UUG")
			protein.push_back('L');
		else if (codon == "GUU" || codon == "GUC" || codon == "GUA" || codon == "GUG")
			protein.push_back('V');
		else if (codon == "UUU" || codon == "UUC")
			protein.push_back('F');
		else if (codon == "AUG")
			protein.push_back('M'); 
		else if (codon == "UGU" || codon == "UGC")
			protein.push_back('C'); 
		else if (codon == "GCU" || codon == "GCC" || codon == "GCA" || codon == "GCG")
			protein.push_back('A');
		else if (codon == "GGU" || codon == "GGC" || codon == "GGA" || codon == "GGG")
			protein.push_back('G');
		else if (codon == "CCU" || codon == "CCC" || codon == "CCA" || codon == "CCG")
			protein.push_back('P');
		else if (codon == "ACU" || codon == "ACC" || codon == "ACA" || codon == "ACG")
			protein.push_back('U');
		else if (codon == "UCU" || codon == "UCC" || codon == "UCA" || codon == "AGU" || codon == "AGC")
			protein.push_back('S');
		else if (codon == "UAU" || codon == "UAC")
			protein.push_back('Y');
		else if (codon == "UGG")
			protein.push_back('W');
		else if (codon == "CAA" || codon == "CAG")
			protein.push_back('Q');
		else if (codon == "AAU" || codon == "AAC")
			protein.push_back('N');
		else if (codon == "CAU" || codon == "CAC")
			protein.push_back('H');
		else if (codon == "GAA" || codon == "GAG")
			protein.push_back('E');
		else if (codon == "GAU" || codon == "GAC")
			protein.push_back('D');
		else if (codon == "AAA" || codon == "AAG")
			protein.push_back('K');
		else if (codon == "CGU" || codon == "CGC" || codon == "CGA" || codon == "CGG"  || codon == "AGA" || codon == "AGG")
			protein.push_back('R');
		else if (codon == "UAA" || codon == "UAG" || codon == "UGA") {
			// Stop codon
			return protein;
		}
		else {
			cout << "Unexpected codon found." << endl;
			throw(1);
		}
	}
	
	cout << "Warning: No stop codon found" << endl;
	return protein;
}
