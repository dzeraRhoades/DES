#include "CTR.h"
#include<bitset>
#include<iostream>

CTR::~CTR()
{
	if (_IV != nullptr)
		delete[] _IV;
}

void CTR::encrypt(std::ifstream& inputStream, std::ofstream& outputStream)
{
	if (!(inputStream.is_open() && outputStream.is_open()))
		throw std::exception("the streams can't be null");
	if (algorithm == nullptr)
		throw std::exception("the algorithm is null");

	if (_IV == nullptr)
		generateInitVec();

	uint8_t buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	uint64_t curSize = 0;
	int blockSize = (int)algorithm->getBlockSize() / 8;
	uint8_t* IV = (uint8_t*)alloca(blockSize);
	memset(IV, 0, blockSize);
	memcpy(IV, _IV, blockSize / 2);
	uint8_t c;
	if (_key.empty())
		generateKey();
	inputStream.seekg(0, std::ios::end);
	uint64_t fileSize = inputStream.tellg();
	inputStream.seekg(0, std::ios::beg);
	while (curSize < fileSize)
	{
		inputStream.read((char*)buf, blockSize);
		algorithm->encrypt(IV, _key.data());
		if (fileSize - curSize < blockSize)
		{
			blockSize = fileSize - curSize;
		}
		_xor(buf, IV, blockSize);
		//std::cout << std::bitset<64>(*((uint64_t*)buf)) << std::endl;
		outputStream.write((char*)buf, blockSize);
		incIV(IV);
		curSize += blockSize;
	}
}

void CTR::decrypt(std::ifstream& inputStream, std::ofstream& outputStream)
{
	encrypt(inputStream, outputStream);
}

void CTR::generateInitVec()
{
	//заглушка
	const uint64_t iv = 15981209512059102924;
	uint64_t blockSize = algorithm->getBlockSize();
	uint32_t bytes = blockSize / 8; // block size in bytes
	_IV = new uint8_t[bytes];
	memset(_IV, 0, bytes);
	memcpy(_IV, &iv, 4);
}

void CTR::incIV(uint8_t* iv)
{
	uint32_t blockSize = algorithm->getBlockSize();
	uint32_t bytes = blockSize / 8; // block size in bytes
	uint8_t* curValue = iv + bytes - 1; // last 8 bits
	while ((*curValue) == 255)
		curValue--; // move pointer left
	++(*curValue);
}

void CTR::_xor(uint8_t* buf, uint8_t* iv, int size) const
{
	//std::cout << std::bitset<64>(*((uint64_t*)buf)) << std::endl;
	//std::cout << std::bitset<64>(*((uint64_t*)iv)) << std::endl;
	for (int i = 0; i < size; ++i)
	{
		buf[i] ^= iv[i];
	}
	//std::cout << std::bitset<64>(*((uint64_t*)buf)) << std::endl;
}


void CTR::generateKey()
{
	if (algorithm == nullptr)
		throw std::exception("the algorithm is null");

	// заглушка
	uint64_t k = 1294612984012712214;
	_key.reserve(algorithm->getKeySize() / 8);
	// log determines the binary length of k
	while (k)
	{
		_key.push_back((uint8_t)k);
		k >>= 8;
	}
}
