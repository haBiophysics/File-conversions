#include <iostream>
#include <vector>
#include <fstream>
#include <sys/stat.h>

using namespace std;


// Converts FASTQ read quality indicated by a character to an integer value in range [0,93]
int char2weight (const char x) {
	int value = (int) x;
	if (x < 33 || x > 126)
		throw("Unexpected quality value in fastq format.\n");
	return value-33;
}


vector <int> char2weight (const string& x) {
	int n = x.size();
	vector <int> out(n);
	for (int i=0; i<n; i++)
		out.at(i) = char2weight (x.at(i));
	return out;
}


char int2quality (const int x) {
	if (x < 33 || x > 126)
		throw("Unexpected quality value in fastq format.\n");
	else
		return (char) x+33;
}


string int2quality (const vector <int>& x) {
	int n = x.size();
	string out;
	for (int i=0; i<n; i++)
		out.push_back( int2quality (x.at(i)) );
	return out;
}


int averageQualityScore (const string& quality) {
	int score = 0;
	for (int i=0; i<quality.size(); i++) {
		score += char2weight(quality.at(i));
	}
	
	return score/quality.size();
}


// Generates the reverse-complementary of a given strand
string complementary (const string& x) {
	string out;
	out.reserve(x.size());
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
			case 'N':
				temp = 'N';
				break; 
			default:
				cout << x.at(i) << endl;
				throw("Unexpected base " + x + " in a DNA molecule.\n");
		}
		out.push_back(temp);
	}
	return out;
}


// Individual sequences obtained from a high throughput device.
class seqread {
	public:
	string rawseq; // Whole sequence as it is
	vector <int> quality; // quality factors converted to int. The higher the better.
	int flag; // 0:Unprocessed or OK, 1:Processing started, 2: Read too short to process, 3: Cannot localise the adaptor ...
	
	seqread (const string& rawseq_, const string& rawquality_);
	seqread (void);
	seqread (const seqread&);
	~seqread(void);
	int size (void) const;
};


seqread::seqread (const string& rawseq_, const string& rawquality_) {
	rawseq = rawseq_;
	quality = char2weight(rawquality_);
}


seqread::seqread (void) {
}


seqread::seqread (const seqread& x) {
	rawseq = x.rawseq;
	quality = x.quality;
	flag = x.flag;
}


seqread::~seqread (void) {
}


int seqread::size(void) const {
	return this->rawseq.size();
}


seqread complementary (const seqread& in) {
	seqread out;
	out.rawseq = complementary(in.rawseq);
	
	for (int i=in.size()-1; i>=0; i--)
		out.quality.push_back( in.quality.at(i) );
	
	return out;
}


size_t getFilesize(const char* filename) {
	struct stat st;
	if(stat(filename, &st) != 0)
		return 0;
	return st.st_size;
}


class FASTQfile {
	public:
	string filename;
	string filenameReverse; // For paired end sequencing
	vector <seqread> reads;
	vector <seqread> reverseReads; // Note: stored as reverse complemented during importation
	int readLengthForward;
	int readLengthReverse;

	FASTQfile (const string filename_);
	FASTQfile (const string filenameFW, const string filenameRV);
	FASTQfile (const FASTQfile& x);
	FASTQfile (string filename_, bool QCfunc (const string&) );
	~FASTQfile (void);
	
	int size();
	void resize(const int);
	void clear(void);
	void append (const string& sequence, const string& qscore);
	void writeout(const string& outfilename);
};


// Imports the file during initialization. Note, if the input fastq file is too big for the available RAM, may cause an out of memory error.
FASTQfile::FASTQfile (string filename_, bool QCfunc (const string&) ) {
	filename = filename_;
	
	fstream inputFile;
	inputFile.open(filename.c_str());
	if (inputFile.is_open() == false)
		throw("Error: File " + filename + " could not be read.\n");
	
	// Pre-allocate some space to improve performance.
	int estimatedNumReads = getFilesize(filename.c_str()) / (2*100); // 100 bp read is assumed, underestimation is better than overestimation for performance, but in no case it is fatal.
	reads.reserve(estimatedNumReads);
	
	// Import reads 4-line blocks at a time
	int ignoredCounter = 0;
	while (true) {
		string trash;
		getline(inputFile, trash); // Discard info row
		if (inputFile.eof()==true)
			break;
		
		string sequence, qscore;
		getline(inputFile, sequence);
		getline(inputFile, trash); // discard '+' row
		getline(inputFile, qscore);
		
		if (qscore.size() != sequence.size()) {
			cout << "ERROR: Invalid FASTQ file format." << endl;
			throw(1);
		}
		
		// Check the quality scores by applying the given quality function. If passes QC, record. Mark to be ignored if a bad read.
		// Quality check function typically returns true unless the total belows a threshold.
		// If does not pass QC, ignore.
		if (QCfunc(qscore) == true) {
			this->append(sequence, qscore);
		}
		else {
			ignoredCounter++;
		}
	}
	inputFile.close();
	
	// Release excess memory that was previously allocated.
	reads.reserve(reads.size());
	
	cout << ignoredCounter << " out of " << this->size() + ignoredCounter << " reads were discarded due to low quality scores." << endl;
	return;
}


void FASTQfile::append (const string& sequence, const string& qscore) {
	seqread cread (sequence, qscore);
	cread.flag = true;
	this->reads.push_back(cread);
	return;
}


// Dummy QC function
bool returnTrue (const string& x) {
	return true;
}


// If no quality function provided, assume every read is acceptable.
FASTQfile::FASTQfile (string filename_) {
	FASTQfile temp (filename_, returnTrue);
	*this = temp;
	temp.clear();
}


// Paired end reads in two separate files
FASTQfile::FASTQfile (const string filenameFW, const string filenameRV) {
	FASTQfile tempFW (filenameFW, returnTrue);
	*this = tempFW;
	tempFW.clear();
	//delete &tempFW;

	FASTQfile tempRV (filenameRV);
	this->reverseReads.resize(tempRV.size());
	
	# pragma omp parallel for schedule (dynamic, 1000)
	for (int i=0; i<tempRV.size(); i++) {
		this->reverseReads.at(i) = complementary( tempRV.reads.at(i) );
	}
	
	tempRV.clear();
}


FASTQfile::	FASTQfile (const FASTQfile& x) {
	filename = x.filename;
	reads = x.reads;
}


FASTQfile::~FASTQfile (void) {
	reads.clear();
}



int FASTQfile::size(void) {
	return this->reads.size();
}


// Resizes the fastq file container to a given number of elements.
// Useful to truncate the effective file sizes for trial runs
void FASTQfile::resize (const int newsize) {
	reads.resize(newsize);
	reverseReads.resize(newsize);
}


void FASTQfile::clear(void) {
	reads.clear();
	return;
}


void FASTQfile::writeout (const string& outfilename) {
	ofstream outFile;
	outFile.open(outfilename.c_str());
	
	for (int i=0; i<this->size(); i++) {
		string fakeQuality (reads.at(i).rawseq.size(), 'A');
	
		outFile << "@UNKNWN" << endl;
		outFile << reads.at(i).rawseq << endl;
		outFile << "+" << endl;
		outFile << int2quality(reads.at(i).quality) << endl;
	}
	
	outFile.close();
	return;
}
