#include <endian.h>
#include <cstdio>
#include <zlib.h>

#include "sognoHeader.hpp"

static const uint32_t HEADER_MAGIC_NUMBER = 0x25ffa4ff;
static const size_t HEADER_LEN = 0x20;

SognoPartitionExtractor::SognoPartitionExtractor(unsigned char* rawFileBuffer, size_t fileSize) :
	mIsValid(false),
	mFileSize(fileSize),
	mRawFileBuffer(rawFileBuffer),
	mMainHeader(rawFileBuffer),
	mPartition()
{
	if (mRawFileBuffer)
	{
		if (mMainHeader)
		{
			SognoPartition* currPartition = nullptr;
			size_t offset = mMainHeader.offset();
			do
			{
				currPartition = new SognoPartition(rawFileBuffer+offset, fileSize);
				if (*currPartition)
				{
					offset = currPartition->nextPartitionOffset();
					mPartition.push_back(currPartition);
				}
			} while (*currPartition);

			if (mPartition.size() > 0)
			{
				mIsValid = true;
			}
		}
	}
}

SognoPartitionExtractor::~SognoPartitionExtractor()
{
	for(auto currPartition : mPartition)
	{
		delete currPartition;
	}
}

SognoPartitionExtractor::operator bool() const
{
	return mIsValid;
}

SognoPartition::SognoPartition(unsigned char* rawFileBuffer, size_t fileSize) :
	mIsValid(false),
	mHeader(rawFileBuffer),
	mRawPartition(rawFileBuffer),
	mRawPartitionPayload(nullptr),
	mNextPartitionOffset(0)
{
	if (mHeader)
	{
		mRawPartitionPayload = mRawPartition + mHeader.offset();
		mNextPartitionOffset = mHeader.offset() + mHeader.dataSize() + HEADER_LEN + 4;
		mIsValid = true;
	}
	else
	{
		printf("%s: error while extracting partition\n", __PRETTY_FUNCTION__);
	}
}

SognoPartition::~SognoPartition()
{
}

SognoPartition::operator bool() const
{
	return mIsValid;
}

uint32_t SognoPartition::nextPartitionOffset() const
{
	return mNextPartitionOffset;
}

SognoHeader::SognoHeader(unsigned char* rawFileBuffer) :
	mIsValid(false),
	mVersion(0),
	mSystemId(0),
	mOpCode(0),
	mRomAddr(0),
	mDataSize(0),
	mCrc32(0),
	mOffset(0),
	mBlockLen(0)
{
	uint32_t magicNumber;
	memcpy(&magicNumber, rawFileBuffer, 4);
	magicNumber = le32toh(magicNumber); // to be sure we use the little endian data in the right order on this machine!

	if (HEADER_MAGIC_NUMBER == magicNumber)
	{
		size_t pos = 4;
		uint32_t seperator;
		memcpy(&mVersion, rawFileBuffer+pos, 4); pos += 4;
		memcpy(&mSystemId, rawFileBuffer+pos, 4); pos += 4;
		memcpy(&mOpCode, rawFileBuffer+pos, 4); pos += 4;
		memcpy(&mRomAddr, rawFileBuffer+pos, 4); pos += 4;
		memcpy(&mDataSize, rawFileBuffer+pos, 4); pos += 4;
		memcpy(&mCrc32, rawFileBuffer+pos, 4); pos += 4;

		// the next 4 bytes seems to be zero on any image, maybe a seperator:
		memcpy(&seperator, rawFileBuffer+pos, 4); pos += 4;
		if (seperator == 0)
		{
			// calculate CRC32 and compare them:
			uint32_t calcCrc32 = 0;
			calcCrc32 = crc32(calcCrc32, rawFileBuffer+pos, mDataSize);
			if (calcCrc32 == mCrc32)
			{
				// in main header, we don't have any payload, we directly start with the next header
				if (mRomAddr == 0xffffffff)
				{
					mDataSize = 0;
				}
				memcpy(&mBlockLen, rawFileBuffer+pos+mDataSize, 4); pos += 4; // this value is big endian!!

				mVersion = le32toh(mVersion);
				mSystemId = le32toh(mSystemId);
				mOpCode = le32toh(mOpCode);
				mRomAddr = le32toh(mRomAddr);
				mDataSize = le32toh(mDataSize);
				mCrc32 = le32toh(mCrc32);
				mBlockLen = be32toh(mBlockLen) + HEADER_LEN; // BIG ENDIAN!!
				mOffset = pos; // end of the header!

				// that's not realy proper, but easier than doing it with cout:
				printf("====================\n");
				printf("hdr_version: 0x%08X -> %i\n", mVersion, mVersion);
				printf("system_id:   0x%08X\n", mSystemId);
				printf("opcode:      0x%08X\n", mOpCode);
				printf("address:     0x%08X\n", mRomAddr);
				printf("payload len: 0x%08X -> %i bytes\n", mDataSize, mDataSize);
				printf("crc32:       0x%08X\n", mCrc32);
				printf("calcCrc32:   0x%08X\n", calcCrc32);
				printf("NextBlockLen:0x%08X\n", mBlockLen);
				printf("====================\n");

				mIsValid = true;
			}
			else
			{
				printf("invalid crc32! expected 0x%08X but calculated 0x%08X\n", mCrc32, calcCrc32);
			}
		}
		else
		{
			printf("invalid header seperator: 0x%08X\n", seperator);
		}
	}
	else
	{
		printf("BAD MAGIC NUMBER: 0x%08X\n", magicNumber);
	}
}

SognoHeader::operator bool() const
{
	return mIsValid;
}

uint32_t SognoHeader::offset() const
{
	return mOffset;
}

uint32_t SognoHeader::dataSize() const
{
	return mDataSize;
}
