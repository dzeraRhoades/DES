#include "ECB.h"

void ECB::encrypt(std::ifstream& inputStream, std::ofstream& outputStream)
{
	if (!(inputStream.is_open() && outputStream.is_open()))
		throw std::exception("the streams can't be null");
	if (algorithm == nullptr)
		throw std::exception("the algorithm is null");

	uint8_t buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	int blockSize = algorithm->getBlockSize() / 8;
	uint8_t c;
	if(_key.empty())
		generateKey();
	while (!inputStream.read((char*) buf, blockSize).eof())
	{
		algorithm->encrypt(buf, _key.data());
		outputStream.write((char*) buf, blockSize);
	}
	
}

void ECB::decrypt(std::ifstream& inputStream, std::ofstream& outputStream)
{
	if (!(inputStream.is_open() && outputStream.is_open()))
		throw std::exception("the streams can't be null");
	if (algorithm == nullptr)
		throw std::exception("the algorithm is null");
	if (_key.empty())
		throw std::exception("key isn't set");

	uint8_t buf[BUFSIZ];
	memset(buf, 0, BUFSIZ);
	int fileSize = 0;
	int blockSize = algorithm->getBlockSize() / 8;
	inputStream.seekg(0, std::ios::end);
	fileSize = inputStream.tellg();
	inputStream.seekg(0, std::ios::beg);
	while (!inputStream.read((char*)buf, blockSize).eof())
	{
		algorithm->decrypt(buf, _key.data());
		outputStream.write((char*)buf, blockSize);
	}
}

void ECB::generateKey()
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
