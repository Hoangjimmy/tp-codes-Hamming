#include <iostream>
#include <fstream>

int main() {
	std::ofstream outfile ( "count.txt" );
	
	for ( int i = 0; i < 16; i += 2 ) {
		outfile.put( ( i << 4 ) | i | 1 );
	}
}
