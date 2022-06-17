#include "../pruning/pruningTree.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>
#include <sstream>

#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/filesystem.hpp>
#include "../solver/puzzle.h"

using namespace boost::iostreams;

double PruningStates::estimateSizeInGb() {
	uint64_t siz = ( ( uint64_t ) 1 ) << hashSize;
	double actualSize = double( siz ) / 1000000000.0;
	return actualSize;
}


void PruningStates::performSizeCheck() {
	if ( estimateSizeInGb() > maxSizeInGb ) {
		stringstream ss;
		ss << "Pruning table size exceeds " << maxSizeInGb << " gigabytes, attempted to allocate " << estimateSizeInGb() << " gigabytes";
		throw runtime_error( ss.str() );
	}
}

void PruningStates::insert ( State& state, int moves )
{
    auto s = preHashTransformation(state);
	auto hash = s.toHash() >> discardBits;
    //tableMutex.lock();
	((volatile uint8_t&) data[hash]) = data[hash] < moves ? data[hash] : moves;
    //tableMutex.unlock();
}

bool PruningStates::cannotBeSolvedInLimit( int movesAvailable, const State& state ) {
	if ( depth >= movesAvailable ) {
		auto hash = state.toHash() >> discardBits;
		return data[hash] > movesAvailable;
	}

	return false;
}

void PruningStates::unload() {
	if(data != nullptr){
		log << "Unloading table (" << path << ") from memory\n";
		log << "----------------------------------------------------------------\n";

        delete[] data;

		data = nullptr;
	}
}



void PruningStates::load() {
	threadManager.targetThreads = cfg->targetThreads;
	if(data != nullptr){
		return;
	}

	performSizeCheck();
	initHashMask();
	uint64_t siz = ( ( uint64_t ) 1 ) << hashSize;
	
	if ( path == "" && useMmap ) {
		throw runtime_error( "error: cannot have use a memory mapped file without specifying a file name or path." );
	}

	if ( boost::filesystem::exists( path ) ) {
		if ( !useMmap ) {
			log << "Pruning table (" << path << ") exists, loading it to memory (size=" << estimateSizeInGb() << "Gb)...\n";

            data = new uint8_t[ siz ];

			uint64_t checksum = 0;
			ifstream infile( path, std::ios::binary );

			infile.read( ( char* ) &checksum, sizeof(checksum) );
			if(checksum != getChecksum()){
				throw runtime_error( "error: checksum mismatch for pruning table ("+path+") consider deleting it, or maybe it is from another puzzle?" );
			}
			infile.read( ( char* ) stats.data(), sizeof(stats.data())*stats.size());
			
			
			if ( !( infile.read( ( char* ) data, siz ) ) ) { // read up to the size of the buffer
				if ( !infile.eof() ) {
					stringstream ss;
					ss << "error: table (" << path << ") is not of the exact length.";
					throw runtime_error(ss.str());
				}
			}
		} else {
			log << "Pruning table (" << path << ") exists, opening it as a memory mapped file (size=" << estimateSizeInGb() << "Gb)...\n";
			mapped_file_params params;
			params.path = path;
			params.length = siz;
			params.flags = mapped_file::mapmode::readwrite;

			file.open( params );

			if ( !file.is_open() ) {
				stringstream ss;
				ss << "error: could not open table (" << path << ") as a memory mapped file.";
				throw runtime_error( ss.str() );
			}

			data = static_cast<uint8_t*>( ( void* )file.data() );
			
			uint64_t checksum = 0; 
			checksum = *((uint64_t*)data);
			data += sizeof(checksum);
			uint64_t* statsPtr = (uint64_t*)data;
			data += sizeof(stats.data())*stats.size();

			for(int i = 0; i < stats.size(); i++){
				stats[i] = statsPtr[i];
			}
			
			if ( checksum != getChecksum() ) {
				throw runtime_error( "error: checksum mismatch for pruning table (" + path + ") consider deleting it, or maybe it is from another puzzle?" );
			}
		}
	} else {
		redundancyTableInverse.load();
		if ( useMmap ) {
			uint64_t checksum = getChecksum(); 
			log << "Creating pruning table (" << path << ") of size " << estimateSizeInGb() <<  "Gb as a memory mapped file...\n\n";
			mapped_file_params params;
			params.path = path;
			params.new_file_size = siz+sizeof(checksum);
			params.length = siz;
			params.flags = mapped_file::mapmode::readwrite;

			file.open( params );

			if ( !file.is_open() ) {
				stringstream ss;
				ss << "error: could not create table (" << path << ") as a memory mapped file.";
				throw runtime_error( ss.str() );
			}

			data = static_cast<uint8_t*>( ( void* )file.data() );
			
			((uint64_t*)data)[0] = checksum;
			data+= sizeof(checksum);
		} else  {
			log << "Creating pruning table (" << path << ") of size " << estimateSizeInGb() <<  " Gb in memory...\n\n";
			data = new uint8_t[ siz ];
		}
		for(uint64_t i = 0; i < siz; i ++) data[i] = depth+1;
		
		generate();

		if ( !useMmap && path != "") {
			log << "\nSaving table (" << path << ").\n";
			ofstream file( path, std::ios::binary );
			uint64_t checksum = getChecksum();
			file.write( ( char* ) &checksum, sizeof(checksum) );
			file.write( ( char* ) stats.data(), sizeof(stats.data())*stats.size());
			file.write( ( char* ) data, siz );
			file.close();
		}
	}
	
	log <<"----------------------------------------------------------------\n";
}

string PruningStates::getStats() { // TODO: store stats so we dont have to look through the whole TABLE (potentially several Gib)
    uint64_t siz = ( ( uint64_t ) 1 ) <<hashSize;
    stringstream ss;
    ss << "maxDepth = " << depth << endl;

	bool needsStats = true;

	for ( int i = 0; i < 256; i++ ) if(stats[i]) needsStats = false;
    
	if(needsStats){
		for ( uint64_t i = 0; i < siz; i++ ) {
			stats[data[i]]++;
		}
		fstream file( path, std::ios::binary | std::ios::in | std::ios::out );
		uint64_t checksum = getChecksum();
		file.write( ( char* ) &checksum, sizeof(checksum) );
		file.write( ( char* ) stats.data(), sizeof(stats.data())*stats.size());
		file.close();
	}

    for ( int i = 0; i < 256; i++ ) {
		if(stats[i])
			ss << "Depth " << i << " has " << stats[i] << "." << endl;
    }
    return ss.str();
}

void PruningStates::initHashMask() {
	discardBits = 64-hashSize;
}

State PruningStates::preHashTransformation( State s) {
	return s;
}

uint64_t PruningStates::getChecksum() {
	const uint64_t goldenRatioConstant = 0x9e3779b97f4a7c15;
	uint64_t result = 0;
	
	result ^= puzzle.getChecksum();
	result ^= hashSize * ( ( goldenRatioConstant << 6 ) | ( goldenRatioConstant >> ( 64 - 6 ) ) );
	result ^= depth * ( ( goldenRatioConstant << 40 ) | ( goldenRatioConstant >> ( 64 - 40 ) ) );

	return result;
}





