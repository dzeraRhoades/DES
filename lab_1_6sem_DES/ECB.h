#pragma once
#include"IBlockEncryptionMode.h"

class ECB : public IBlockEncryptionMode
{
private:
	std::vector<uint8_t> _key;
public:
	explicit ECB(EncryptionBlockAlgorithm const* algorithm) : IBlockEncryptionMode(algorithm) {}
	void encrypt(std::ifstream& inputStream, std::ofstream& outputStream) override;
	void decrypt(std::ifstream& inputStream, std::ofstream& outputStream) override;
private:
	/*
	generate key and write id into key 
	@param key
	*/
	void generateKey();
};

