// Program to align two protein sequences

#include <iostream>
#include <vector>
#include <map>
#include <fstream>

using namespace std;

const int gapOpening = -11;
const int gapExtension = -1;

map <char,int> residue2int;
vector <vector <int> > scoreMatrix;

void importScoringMatrix (string filename) {	
	ifstream infile (filename.c_str(), ios::in);
	scoreMatrix.resize(20);
	
	// Row 1
	string trash;
	getline(infile, trash);
	
	for (int row=0; row<20; row++) {
		scoreMatrix.at(row).resize(20);
		char letterCode;
		infile >> letterCode;
		residue2int[letterCode] = row;
		
		for (int column=0; column<20; column++) {
			int score;
			infile >> score;
			scoreMatrix[row][column] = score;
		}
	}
		
	infile.close();
	return;
}


int matchContribution (char x, char y) {
	return scoreMatrix[residue2int[x]][residue2int[y]];
}


void align (string s, string t) {
	// Initialize the matrix
	vector <vector <int> > matrix (s.size()+1);
	vector <vector <int> > source (s.size()+1); // 0: diagonal, 1: from top, 2: from left
	for (int i=0; i<s.size()+1; i++) {
		source[i].resize(t.size()+1);
		source[i][0] = 1;
		
		matrix[i].resize(t.size()+1);
		if (i<=1)
			matrix[i][0] = gapOpening;
		else
			matrix[i][0] = matrix.at(i-1)[0] + gapExtension;
	}
	
	for (int i=0; i<t.size()+1; i++) {
		source[0][i] = 2;
		if (i<=1)
			matrix[0][i] = gapOpening;
		else
			matrix[0][i] = matrix[0][i-1] + gapExtension;
	}
	source[0][0] = 0;
	matrix[0][0] = 0;	
			
			
	// Fill the matrix, match>left>top is preferred in equality over equivalent scenarios.
	for (int i=1; i<s.size()+1; i++) {
		for (int j=1; j<t.size()+1; j++) {			
			int matchScore = matrix[i-1][j-1] + matchContribution(s[i-1],t[j-1]);
			int leftScore = matrix[i][j-1];
			if (source[i][j-1] == 2)
				leftScore += gapExtension;
			else
				leftScore += gapOpening;
			
			int topScore = matrix[i-1][j];
			if (source[i-1][j] == 1)
				topScore += gapExtension;	
			else
				topScore += gapOpening;	
			
			
			matrix[i][j] = matchScore;
			if (leftScore>matrix[i][j]) {
				matrix[i][j] = leftScore;
				source[i][j] = 2;
			}
			if (topScore>matrix[i][j]) {
				matrix[i][j] = topScore;
				source[i][j] = 1;
			}

		}
	}
	
	// Activate below to see alignment matrix and the corresponding moves
	if (false)  {
		for (int i=0; i<source.size(); i++) {
			for (int j=0; j<source.at(0).size(); j++)
				cout << matrix[i][j] << "\t";
			cout << endl;
		}
		cout << endl;
	
		for (int i=0; i<source.size(); i++) {
			for (int j=0; j<source.at(0).size(); j++)
				cout << source[i][j] << "\t";
			cout << endl;
		}
	}
	
	
	// Traceback and report
	string sout,tout;
	int i=s.size();
	int j=t.size();
	int score = matrix[i][j];
	while ( i>=0 && j>=0) {
		switch (source[i][j]) {
			case 0:
				sout = s[i-1] + sout;
				tout = t[j-1] + tout;
				i--;
				j--;
				break;
			case 1:
				sout = s[i-1] + sout;
				tout = "-" + tout;
				i--;
				break;
			case 2:
				sout = "-" + sout;
				tout = t[j-1] + tout;
				j--;
				break;
		}
	}
	
	cout << score << endl;
	cout << sout << endl;
	cout << tout << endl;
	return;
}
