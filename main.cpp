#include <iostream>      
#include <fstream>
#include <string>         
#include <bitset>        
#include <vector>

#include <cstdlib>
#include <ctime>
#include <limits>

/**
 * Hamming (7,4)
 **/ 
#define N 4
#define HAMMING_7 7

/**
 * Debug macros
 **/ 
#define DEBUG_RF true // Debug Information: Read File
#define DEBUG_HE true // Debug Information: Hamming Encoding

using namespace std; 

/**
 * vector<bitset<N> > readFile(string filename)
 * Read a file in binary and create a vector of bitset wih a width of 4 for each bitset
 * Return a vector bitset
 **/ 
vector<bitset<N> > readFile( string filename )
{
	vector<bitset<N> > content;
	ifstream reader;
	char buffer;
	reader.open(filename.c_str(), ios::binary|ios::in);

	if(DEBUG_RF)
		cout << "Read    :";

	if(reader != NULL && reader.is_open())
	{
		while(!reader.eof())
		{
			reader.read(&buffer, 1);
			bitset<N> bsBufferLSB(buffer);
			bitset<N> bsBufferMSB(buffer>>4);
			
			content.push_back(bsBufferMSB);
			content.push_back(bsBufferLSB);
	
			if(DEBUG_RF)
			{
				cout << " | " << bsBufferMSB.to_string() << "   ";
				cout << " | " << bsBufferLSB.to_string() << "   ";
			}
		}
	}
	
	if(DEBUG_RF)
		cout << endl;

	reader.close();
	return content;
} 

/**
 * vector<bitset<HAMMING_7> > HammingEncoding(vector<bitset<N> > bitsetVector)
 * Convert a vector of bitset<4> into a hamming vector of bitset<7>
 **/ 
vector<bitset<HAMMING_7> > HammingEncoding(vector<bitset<N> > bitsetVector)
{
	vector<bitset<HAMMING_7> > encodedBitset;
	
	if(DEBUG_HE)
		std::cout << "Encode  :";
		
	for(vector<bitset<N> >::iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)
	{
		// Code to modify (sample)		
		bitset<N> inBuffer = *i;
		bitset<HAMMING_7> outBuffer;

		outBuffer[0] = inBuffer[0];
		outBuffer[1] = inBuffer[1];
		outBuffer[2] = inBuffer[2];
		outBuffer[3] = inBuffer[3];
		
		outBuffer[4] = 0;
		outBuffer[5] = 0;
		outBuffer[6] = 0;
		
		if(DEBUG_HE)
			cout << " | " << outBuffer.to_string();
		
		encodedBitset.push_back(outBuffer);
	}
	
	if(DEBUG_HE)
		cout << endl;
	
	return encodedBitset;
}

const vector<bitset<HAMMING_7>>& Hamming7_4Generator() {
	static vector<bitset<HAMMING_7>> generator;
	static bool initialized = false;
	
	if ( ! initialized ) {
		generator.push_back( bitset<HAMMING_7>( 0x07 ) );
		generator.push_back( bitset<HAMMING_7>( 0x19 ) );
		generator.push_back( bitset<HAMMING_7>( 0x2A ) );
		generator.push_back( bitset<HAMMING_7>( 0x4B ) );
	}

	return generator;
}


template <size_t DEC_W, size_t ENC_W>
bitset<ENC_W> GeneratorHammingEncodeSymbol( const bitset<DEC_W>& inBuffer, const vector<bitset<ENC_W>>& generator ) {
	bitset<ENC_W> outBuffer = 0;

	for ( size_t i = 0; i < DEC_W; ++i ) {
		if ( inBuffer[DEC_W-1-i] != 0 ) {
			outBuffer ^= generator[i];
		}
	}
	
	if(false&&DEBUG_HE)
		cout << " | " << outBuffer.to_string();
	
	return outBuffer;
}

/**
 * 
 **/
template <size_t DEC_W, size_t ENC_W>
vector<bitset<ENC_W> > GeneratorHammingEncoding( vector<bitset<DEC_W> > bitsetVector, const vector<bitset<ENC_W>>& generator )
{
	vector<bitset<HAMMING_7> > encodedBitset;
	
	if(DEBUG_HE)
		std::cout << "Encode  :";
		
	for(vector<bitset<N> >::iterator i = bitsetVector.begin(); i != bitsetVector.end();++i)
	{
		encodedBitset.push_back( GeneratorHammingEncodeSymbol( *i, generator ) );
	}
	
	if(DEBUG_HE)
		cout << endl;
	
	return encodedBitset;
}

vector<bitset<N>> GeneratorHammingDecoding7_4( vector<bitset<HAMMING_7> > bitsetVector )
{
	vector<bitset<N> > encodedBitset;
	
	if(DEBUG_HE)
		std::cout << "Decode  :";
		
	for(vector<bitset<HAMMING_7> >::const_iterator it = bitsetVector.begin(); it != bitsetVector.end(); ++it)
	{
		bitset<HAMMING_7> inBuffer = *it;
		
		bitset<HAMMING_7-N> syndrom = 0;
		for ( int i = 0; i < HAMMING_7; ++i ) {
			if ( inBuffer[i] != 0 ) {
				syndrom ^= bitset<HAMMING_7-N>( i + 1 );
			}
		}
		
		int intSyndrom = syndrom.to_ulong();
		
		if(DEBUG_HE)
			cout << " | " << intSyndrom;
			
		if ( intSyndrom != 0 ) {
			inBuffer[intSyndrom-1] = ! inBuffer[intSyndrom-1];
		}
		
		bitset<N> outBuffer = 0;
		
		outBuffer[0] = inBuffer[6];
		outBuffer[1] = inBuffer[5];
		outBuffer[2] = inBuffer[4];
		outBuffer[3] = inBuffer[2];
		
		if(DEBUG_HE)
			cout << "  " << outBuffer.to_string();
		
		encodedBitset.push_back(outBuffer);
	}
	
	if(DEBUG_HE)
		cout << endl;
	
	return encodedBitset;
}

template <size_t W>
size_t HammingDistance( const bitset<W>& a, const bitset<W>& b ) {
	size_t res = 0;
	for ( size_t i = 0; i < W; ++i ) {
		res += a[i] != b[i];
	}
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                     Main                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define ERROR_PROP 0.01

int main_encoder( string fileName ) {
	vector< bitset<N> > input_data;
	vector< bitset<HAMMING_7> > encoded_data;
	vector< bitset<HAMMING_7> > errored_data;
	vector< bitset<N> > decoded_data;

	// Read data to encode
	input_data = readFile( fileName );

	// Encode by Hamming (7,4) coding
	encoded_data = GeneratorHammingEncoding<4,7>( input_data, Hamming7_4Generator() );

	// Inject error
	srand( time( NULL ) );
	cout << "Errors  :";
	for ( vector<bitset<HAMMING_7>>::const_iterator it = encoded_data.begin(); it != encoded_data.end(); ++it ) {
		bitset<HAMMING_7> data = *it;
		cout << " | ";
		for ( int i = HAMMING_7-1; i >= 0; --i ) {
			if ( rand() / (float) RAND_MAX < ERROR_PROP ) {
				data[i] = ! data[i];
				cout << '#';
			} else {	
				cout << '_';
			}
		}
		errored_data.push_back( data );
	}
	cout << endl;

	// Decode
	decoded_data = GeneratorHammingDecoding7_4( errored_data );
	
	int errors = 0;
	int position = 0;
	for ( vector<bitset<N>>::const_iterator it = input_data.begin(); it != input_data.end(); ++it, ++position ) {
		errors += HammingDistance<N>( *it, decoded_data[position] );
	}
	cout << "Errors: " << errors << endl;
	cout << "Bits: " << input_data.size() * N << endl;
	
	return 0;
}

int computeDistance( const vector<bitset<HAMMING_7>>& generator ) {
	vector< bitset<HAMMING_7> > codes;
	size_t len = 1 << N;
	codes.reserve( len );
	for ( size_t i = 0; i < len; ++i ) {
		codes.push_back( GeneratorHammingEncodeSymbol( bitset<N>( i ), generator ) );
	}
	
	int minDist = numeric_limits<int>::max();
	
	for ( size_t i = 0; i < len - 1; ++i ) {
		for ( size_t j = i + 1; j < len; ++j ) {
			int dist = HammingDistance( codes[i], codes[j] );
			if ( dist < minDist ) {
				minDist = dist;
			}
		}
	}
	
	return minDist;
}

int main_distance() {
	cout << "minimal distance: " << computeDistance( Hamming7_4Generator() ) << endl;
	
	return 0;
}

int main( int argc, char* argv[] ) {
	switch ( argc ) {
		case 1: return main_distance();
		case 2: return main_encoder( argv[1] );
		default:
			cerr << "Unexpected arguments. Valid calls:" << endl
				<< " ⋅ " << argv[0] << "             Compute minimal Hamming distance" << endl
				<< " ⋅ " << argv[0] << " fileName    Encode <fileName>" << endl;
			return 1;
	}
}
