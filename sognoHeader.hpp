#ifndef SOGNOHEADER_HPP
#define SOGNOHEADER_HPP

#include <cinttypes>
#include <cstring>
#include <vector>

class SognoHeader
{
	bool mIsValid;
	uint32_t mVersion; // version of this header
	uint32_t mSystemId; // to check the receiver model?
	uint32_t mOpCode;
	uint32_t mRomAddr;
	uint32_t mDataSize;
	uint32_t mCrc32; // crc32 of payload
	size_t mOffset;
	uint32_t mBlockLen;

	public:
			SognoHeader(unsigned char*);

			explicit operator bool() const;
			uint32_t offset() const;
			uint32_t dataSize() const;
};

class SognoPartition
{
	bool mIsValid;
	SognoHeader mHeader;
	unsigned char* mRawPartition; // with header
	unsigned char* mRawPartitionPayload; // only payload
	uint32_t mNextPartitionOffset;

	public:
			SognoPartition(unsigned char*, size_t);
			~SognoPartition();

			explicit operator bool() const;
			uint32_t nextPartitionOffset() const;
};

class SognoPartitionExtractor
{
	bool mIsValid;
	size_t mFileSize;
	unsigned char* mRawFileBuffer;

	SognoHeader mMainHeader; // header of the whole image
	std::vector<SognoPartition*> mPartition;

	public:
			SognoPartitionExtractor(unsigned char*, size_t);
			~SognoPartitionExtractor();

			explicit operator bool() const;
};

#endif
