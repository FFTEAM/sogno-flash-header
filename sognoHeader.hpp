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

	SognoPartition(const SognoPartition&) = delete;
	const SognoPartition& operator=(const SognoPartition&) = delete;

	public:
			SognoPartition(unsigned char*, ssize_t);
			~SognoPartition();

			explicit operator bool() const;
			uint32_t nextPartitionOffset() const;
			bool writeData(const std::string&, size_t) const;
};

class SognoPartitionExtractor
{
	bool mIsValid;
	ssize_t mFileSize;
	unsigned char* mRawFileBuffer;

	SognoHeader mMainHeader; // header of the whole image
	std::vector<SognoPartition*> mPartition;

	SognoPartitionExtractor(const SognoPartitionExtractor&) = delete;
	const SognoPartitionExtractor& operator=(const SognoPartitionExtractor&) = delete;

	public:
			SognoPartitionExtractor(unsigned char*, ssize_t);
			~SognoPartitionExtractor();

			explicit operator bool() const;
			bool writePartitions(const std::string&) const;
};

#endif
