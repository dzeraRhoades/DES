#pragma once
#include<vector>

class EncryptionBlockAlgorithm
{
public:
	EncryptionBlockAlgorithm() = default;
	virtual ~EncryptionBlockAlgorithm() = default;
	virtual void encrypt(uint8_t* text, const uint8_t* key) const = 0;
	virtual void decrypt(uint8_t* text, const uint8_t* key) const = 0;
	virtual uint64_t getBlockSize() const = 0;
	virtual uint64_t getKeySize() const = 0;
};

