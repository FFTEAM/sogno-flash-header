#include <iostream>
#include <fstream>

#include "sognoHeader.hpp"

int main()
{
	std::fstream imageFile;
	imageFile.open("image.dmp", std::fstream::in);
	if (!imageFile.is_open())
	{
		std::cerr << "could not open image file!" << std::endl;
		return 1;
	}

	imageFile.seekg (0, imageFile.end);
	ssize_t length = imageFile.tellg();
    imageFile.seekg (0, imageFile.beg);
    std::cout << "opened image file with " << length << " bytes" << std::endl;

    // let's read the file into memory:
    unsigned char* rawFileBuffer = new unsigned char[length];
    if (!rawFileBuffer)
    {
		std::cerr << "memory allocation error!" << std::endl;
		return 1;
	}
	imageFile.read(reinterpret_cast<char*>(rawFileBuffer), length);
	imageFile.close();
	// data is stored in little endian!

	SognoPartitionExtractor partitionExtractor(rawFileBuffer, length);
	if (!partitionExtractor)
	{
		std::cerr << "partition extraction failed due an error!" << std::endl;
		return 1;
	}

	if (!partitionExtractor.writePartitions("prefix"))
	{
		std::cerr << "writing partitions failed!" << std::endl;
		return 1;
	}

	delete[] rawFileBuffer;
	return 0;
}
