#pragma once

#ifndef logging_hpp
#define logging_hpp

#include<vector>
#include<iostream>

using namespace std;

class Logging {
private:
	vector<ostream*> fwd = {};
public:
	bool wasUsed = false;
	template<typename T>
	Logging& operator<<( T s ) {
		for ( auto ostrm : fwd ) {
			( *ostrm ) << s;
		}

		wasUsed = true;
		return *this;
	}
	Logging& operator<< ( ios & ( *pf )( ios& ) ) {
		for ( auto ostrm : fwd ) {
			( *ostrm ) << pf;
		}

		wasUsed = true;
		return *this;
	}
	Logging& operator<< ( ios_base & ( *pf )( ios_base& ) ) {
		for ( auto ostrm : fwd ) {
			( *ostrm ) << pf;
		}

		wasUsed = true;
		return *this;
	}
	Logging& operator<< ( ostream & ( *pf )( ostream& ) ) {
		for ( auto ostrm : fwd ) {
			( *ostrm ) << pf;
		}

		wasUsed = true;
		return *this;
	}
	void forward( ostream& ostrm ) {
		fwd.push_back( &ostrm );
	}
};

#endif
