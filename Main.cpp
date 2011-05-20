// Main.cpp

// This is just a stub
#include <iostream>

//#define USE_STD_CXX_INCLUDES 1

// Test out the includes
// DCMTK includes
#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dctagkey.h"

int main()
{
	DcmTagKey patName( 0x0010, 0x0010 );
	std::cout << "Yes, we can write without link errors" << std::endl;
	std::cout << "PatName tag converted toString() [" << patName.toString() << "]" << std::endl;


	return 0;
}
