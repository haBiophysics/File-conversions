// An experimental program to solve Sudoku puzzles
// Tunc Kayikcioglu 2017

#include <iostream>
#include <vector>
#include <sstream>

using namespace std;



vector<int> allowedNumbers (const vector <vector <int> >& in, const int& i, const int& j) {
	vector <bool> numbers (10);
	for (int k=0; k<9; k++) {
		numbers[ in[i][k] ] = true;
		numbers[ in[k][j] ] = true;
	}
	
	// 3x3 block should not have this number
	for (int k=0; k<3; k++)
		for (int l=0; l<3; l++)
			numbers[ in[3*(i/3)+k][3*(j/3)+l] ] = true;	
	
	vector <int> out;
	for (int k=1; k<10; k++)
		if(numbers[k] == false)
			out.push_back(k);
	
	return out;
}



// Sanity check on a table with given list of inputs
bool verifyBoard (vector <vector <int> > in) {
	for (int i=0; i<9; i++) {
		for (int j=0; j<9; j++) {
			if (in[i][j] <0 || in[i][j]>9)
				return false;
		}
	}

	for (int i=0; i<9; i++) {
		for (int j=0; j<9; j++) {
			int temp = in[i][j];
			if (temp != 0) {
				in[i][j] = 0;
				vector<int> isAllowed = allowedNumbers(in,i,j);
				bool OKflag = false;
				for (int k=0; k<isAllowed.size(); k++)
					if(isAllowed[k] == temp) {
						OKflag = true;
						break;
					}
				if (OKflag == false)
					return false;
				in[i][j] = temp;
			}
		}
	}

	return true;
}



vector <vector <int> > solveBoard (vector <vector <int> > in) {
	vector <vector <vector <int> > > possibilities (9);
	bool noSolution = false;
	bool isComplete = true;
	
	#pragma omp parallel for
	for (int i=0; i<9; i++) {
		possibilities[i].resize(9);
		for (int j=0; j<9 && noSolution==false; j++) {
			if (in[i][j] == 0) {
				isComplete = false;
				possibilities[i][j] = allowedNumbers(in,i,j);
				if (possibilities[i][j].size() == 0) {
					noSolution = true;
				}
			}
		}
	}
	
	
	// All entries are known
	if (isComplete == true)
		return in;
	
	// Inconsistent
	if (noSolution == true) {
		in.clear();
		return in;
	}
	
	
	bool hitFlag = false;
	#pragma omp parallel for
	for (int i=0; i<9; i++) {
		for (int j=0; j<9; j++) {
			if (possibilities[i][j].size() == 1) {
				// Pre-determined entry detected, record it.
				in[i][j] = possibilities[i][j][0];
				hitFlag = true;
			}
		}
	}
	if (hitFlag == true)
		return solveBoard(in);
	
	
	// No pre-determined entry detected, trial and error.
	for (int options = 2; options <10; options++) {
		for (int i=0; i<9; i++) {
			for (int j=0; j<9; j++) {
				if (possibilities[i][j].size() == options) {
					for (int k=0; k<options; k++) {
						in[i][j] = possibilities[i][j][k];
						vector <vector <int> > temp = solveBoard(in);
						if (temp.size() != 0)
							return temp;
					}
				}
			}
		}
	}
	
	
	// This sudoku is does not have any solutions.
	in.clear();
	return in;
}



int main (int argc, const char** argv) {
	if (argc != 82) {
		cout << "Incomplete matrix provided." << endl;
		return -1;
	}
	
	// get input matrix
	vector <vector <int> > start (9);
	for (int i=0; i<9; i++) {
		start.at(i).resize(9);
	}
	for (int i=0; i<81; i++) {
		stringstream temp (argv[i+1]);
		temp >> start[i%9][i/9];
	}

	// Check for self-consistency
	if (verifyBoard(start) == false) {
		cout << "-2 Invalid input matrix." << endl;
		return -2;
	}
	
	// Solve
	vector <vector <int> > solution = solveBoard(start);
	if (solution.size() == 0) {
		cout << "-3 No solution possible: inconsistent Sudoku." << endl;
		return -3;
	}

	// Report
	cout << 0 << " ";
	for (int i=0; i<81; i++) {
		cout << solution[i%9][i/9] << " ";
	}
	cout << endl;
	return 0;
}


