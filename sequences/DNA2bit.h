// Attempt to represent bases as 2-bit variables, rather than 1 char (=8 bit) each.
// Not worth using for ordinary applications as the performance gain is only about 2 fold.


#include <iostream>
#include <cstring>

#define DNA_BUFFER_LENGTH 64


using namespace std;


class DNA2bit {
	public:
		bool bases[DNA_BUFFER_LENGTH];
		int size;
		
		DNA2bit (const string& x);
		DNA2bit (void);
		DNA2bit (const DNA2bit& x);
		~DNA2bit();
		string toString (void);
};



DNA2bit::DNA2bit (const string& x) {
	for (int i=0; i<x.size(); i++) {
		switch (x.at(i)) {
			case 'A':
			case 'a':
				bases[2*i] = 0;
				bases[2*i+1] = 0;
				break;
			case 'C':
			case 'c':
				bases[2*i] = 0;
				bases[2*i+1] = 1;
				break;
			case 'G':
			case 'g':
				bases[2*i] = 1;
				bases[2*i+1] = 0;
				break;
			case 'T':
			case 't':
				bases[2*i] = 1;
				bases[2*i+1] = 1;
				break;
		}
	}
	size = x.size();
}



DNA2bit::DNA2bit (void) {
	size = 0;
}



DNA2bit::DNA2bit (const DNA2bit& x) {
	memcpy(this->bases, x.bases, DNA_BUFFER_LENGTH);
	size = x.size;
}



DNA2bit::~DNA2bit() {
}



string DNA2bit::toString(void) {
	string out;
	out.resize(this->size);
	
	for (int i=0; i<size; i+=2) {
		if ( bases[i] == 0 && bases[i+1] == 0 ) 
			out[i] = 'A';
		else if ( bases[i] == 0 && bases[i+1] == 1 ) 
			out[i] = 'C';	
		else if ( bases[i] == 1 && bases[i+1] == 0 ) 
			out[i] = 'G';
		else
			out[i] = 'T';
	}
	
	return out;
}



bool operator < (const DNA2bit& x, const DNA2bit& y) {
	if (x.size == y.size) {
		for (int i=0; i<x.size; i++) {
			if (x.bases[i] > y.bases[i])
				return false;
		}
	}
	else if (x.size > y.size)
		return false;
		
	return true;
}
