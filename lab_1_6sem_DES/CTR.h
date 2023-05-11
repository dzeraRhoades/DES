#pragma once
#include"IBlockEncryptionMode.h"

class CTR : public IBlockEncryptionMode
{
private:
	std::vector<uint8_t> _key;
	uint8_t* _IV = nullptr;
public:
	explicit CTR(EncryptionBlockAlgorithm* algorithm) : IBlockEncryptionMode(algorithm) {}
	~CTR() override;
	void encrypt(std::ifstream& inputStream, std::ofstream& outputStream) override;
	void decrypt(std::ifstream& inputStream, std::ofstream& outputStream) override;
private:
	void generateInitVec();
	void incIV(uint8_t* iv);
	void _xor(uint8_t* buf, uint8_t* iv, int size) const;
	/*
	generate key and write id into key
	@param key
	*/
	void generateKey();
};

