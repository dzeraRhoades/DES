#pragma once
#include"EncryptionAlgorithm.h"
#include<fstream>

class IBlockEncryptionMode
{
protected:
	const EncryptionBlockAlgorithm* algorithm = nullptr;
public:
	IBlockEncryptionMode(const EncryptionBlockAlgorithm* algorithm)
	{
		this->algorithm = algorithm;
	}
	virtual ~IBlockEncryptionMode() = default;

	/*
	* encrypt input stream and write to output stream
	*/
	virtual void encrypt(std::ifstream& inputStream, std::ofstream& outputStream) = 0;
	/*
	* decrypt input stream and write to output stream
	*/
	virtual void decrypt(std::ifstream& inputStream, std::ofstream& outputStream) = 0;
	
};

