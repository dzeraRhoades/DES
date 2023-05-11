#include<iostream>
#include "DES.h"

//#define LOG(x) std::cout<<x<<std::endl
//#define LOGB(descr, x, x2) std::cout<<descr<<std::bitset<x2>(x)<<std::endl
//#define LOGBI(descr, x, x2) std::cout<<descr; for(int f = 0; f < 64; ++f) std::cout<<f % 10; std::cout<<std::endl; std::cout<<descr<<std::bitset<x2>(x)<<std::endl
#define LOG(x)
#define LOGB(descr, x, x2)
#define LOGBI(descr, x, x2)

DES::DES()
{
	keyGenerator = std::make_unique<DESKeyExpansion>();
}

void DES::encrypt(uint8_t* text, const uint8_t* key) const
{
	std::vector<uint8_t> result;
	uint64_t _text = *(const uint64_t*) text;
	uint64_t _key = *(const uint64_t*) key;
	LOG("start encrypt values: ");
	LOGB("text: ", _text, 64);
	LOGB("key: ", _key, 64);


	std::bitset<56> key_56 = shortenKey(_key);

	LOG("SHORTEN KEY: ");
	LOGB("key: ", key_56.to_ullong(), 64);

	std::bitset<48> key_48;

	// initial permutation
	permutation(initialPermArr, _text);

	LOG("INITIAL PERMUTATION");
	LOGB("text: ", _text, 64);

	for (int i = 0; i < roundsNum; ++i)
	{
		LOG(i << " ROUND");
		uint64_t value = key_56.to_ullong();
		key_48 = std::bitset<48>(keyGenerator->generateRoundKey(i, (uint8_t*)&value, keyShiftArr, keyPermArr)[0]);
		_text = feistelNetwork(key_48, _text);

		LOG(i << " ROUND");
		LOGB("text: ", _text, 64);
		LOGB("key48: ", key_48.to_ullong(), 64);
	}
	// final permutation
	permutation(finalPermArr, _text);
	LOG("FINAL PERMUTATION");
	LOGB("text: ", _text, 64);
	// copy the result to output parameter
	memcpy(text, &_text, sizeof(_text));
}

void DES::decrypt(uint8_t* text, const uint8_t* key) const
{
	std::vector<uint8_t> result;
	uint64_t _text = *(const uint64_t*)text;
	uint64_t _key = *(const uint64_t*)key;

	//TODO: test if bitsets are right

	LOG("start decrypt values: ");
	LOGB("text: ", _text, 64);
	LOGB("key: ", _key, 64);

	// TODO: убрать возвращаемое значение и вносить значение в сам _key
	std::bitset<56> key_56 = shortenKey(_key);
	std::bitset<48> key_48;

	LOG("SHORTEN KEY: ");
	LOGB("key: ", key_56.to_ullong(), 64);

	// final permutation
	permutation(initialPermArr, _text);
	LOG("FINAL PERMUTATION");
	LOGB("text: ", _text, 64);
	
	for (int i = 0; i < roundsNum; ++i)
	{
		LOG(i << " ROUND");
		uint64_t value = key_56.to_ullong();
		key_48 = std::bitset<48>(keyGenerator->generateRoundKey(roundsNum - i - 1, (uint8_t*)&value, keyShiftArr, keyPermArr)[0]);
		_text = feistelNetworkReversed(key_48, _text);
		LOG(i << " ROUND");
		LOGB("text: ", _text, 64);
		LOGB("key48: ", key_48.to_ullong(), 64);
	}
	// initial permutation
	permutation(finalPermArr, _text);
	LOG("INITIAL PERMUTATION");
	LOGB("text: ", _text, 64);
	// copy the result to output parameter
	memcpy(text, &_text, sizeof(_text));
}

std::bitset<48> DES::expandBitset(const std::bitset<32>& bitset) const
{
	std::bitset<48> res(0);
	uint16_t wideLen = 48;
	uint16_t narrowLen = 32;
	for (int j = 1, i = 0; i < narrowLen; ++i, ++j)
	{
		res.set(j % wideLen, bitset.test(i));
		if ((i + 1) % 4 == 0)
		{
			j++;
			res.set(j % wideLen, bitset.test((i + 1) % narrowLen));
			j++;
			res.set(j % wideLen, bitset.test(i % narrowLen));
		}
	}
	return res;
}

void DES::permutation(const std::vector<uint8_t>& array, uint64_t& bits, bool straight)
{
	uint64_t result = 0;
	for (int i = 0; i < array.size(); ++i)
	{
		if (straight) {
			result |= ((bits >> (array[i] - 1)) & 1) << i;
			//LOGBI("text: ", result, 64);
		}
		else
		{
			result |= ((bits & 1) << (array[i] - 1));
			bits >>= 1;
			//LOGBI("text: ", result, 64);
		}
	}
	bits = result;
}

std::bitset<56> DES::shortenKey(const std::bitset<64>& key)
{
	std::bitset<56> res;
	for (int i = 0, j = 0; i < 64; ++i, ++j)
	{
		// каждый восьмой бит отбрасываем
		if ((i + 1) % 8 == 0)
			++i;
		if(i < 64)
			res.set(j, key.test(i));
	}
	return res;
}

uint64_t DES::feistelNetwork(const std::bitset<48>& key, uint64_t text) const
{
	const int pieceSize = 6;
	uint64_t result;
	//get left and right parts of text
	uint64_t old_left = text >> 32;
	std::bitset<32> old_right = (text << 32) >> 32; //delete left 32 bits and then move back
	uint64_t new_right, new_left;

	// feistel func
	new_right = old_left ^ feistelFunc(old_right.to_ullong(), key.to_ullong());

	new_left = old_right.to_ullong();

	LOGB("left text: ", new_left, 64);
	LOGB("right text: ", new_right, 64);

	// merge left and right
	result = (new_left << 32) | new_right;
	return result;
}
uint64_t DES::feistelNetworkReversed(const std::bitset<48>& key, uint64_t text) const
{
	const int pieceSize = 6;
	uint64_t result;
	//get left and right parts of text
	uint64_t old_left = text >> 32;
	std::bitset<32> old_right = (text << 32) >> 32; //delete left 32 bits and then move back
	uint64_t new_right, new_left;

	LOGB("left text: ", old_left, 64);
	LOGB("right text: ", old_right.to_ullong(), 64);
	// feistel func
	new_right = old_left ^ feistelFunc(old_right.to_ullong(), key.to_ullong());

	new_right = old_left;
	new_left = old_right.to_ullong() ^ feistelFunc(old_left, key.to_ullong());

	// merge left and right
	result = (new_left << 32) | new_right;
	return result;
}

uint64_t DES::feistelFunc(const uint64_t& text, const uint64_t& key) const
{
	uint64_t result;
	// expand 
	std::bitset<48> right_48 = expandBitset(text);
	//xor
	right_48 ^= key;
	// S substitution
	result = SSubstitution(right_48.to_ullong());
	return result;
}

uint64_t DES::SSubstitution(uint64_t part) const
{
	uint64_t result = 0;

	int pieceLen = 6; // every piece = 6 bits
	int pieces = 48 / pieceLen;
	uint8_t piece = 0;
	uint8_t row = 0, column = 0;

	for (int i = 0; i < pieces; ++i)
	{
		uint8_t mask = (1 << 6) - 1; // 111111
		piece = mask & part;
		part >>= 6; // delete 6 bits
		row = ((piece & 1) | (piece >> 4)); // take left and right bits
		column = piece & (((1 << 5) - 1) >> 1); // (1 << 5) -2 = 11110, so with & we take only 4 center bits
		uint64_t elem = S[i][row * 16 + column];
		result |= (elem << (i * 4)); // take element from S and move to the left depending on iterayion

	}

	return result;
	
}


