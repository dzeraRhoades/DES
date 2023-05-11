#include <iostream>
#include<memory>
#include<bitset>

#include"DES.h"
#include"ECB.h"
#include"CTR.h"

void test();

int main(int argc, char* argv[])
{
	
	test();

	return 0;
}

void test()
{
	uint64_t text = 12847912749;
	uint64_t key = 13284719471240;
	
	DES des;
	
	des.encrypt((uint8_t*) & text, (uint8_t*)&key);
	des.decrypt((uint8_t*)&text, (uint8_t*)&key);
	//std::cout << text << std::endl;

	std::ifstream input("texts/text.txt", std::ifstream::binary);
	std::ofstream output("texts/cipher.txt", std::ofstream::binary);

	CTR ecb(&des);
	ecb.encrypt(input, output);
	input.close();
	output.close();

	input.open("texts/cipher.txt", std::ifstream::binary);
	output.open("texts/result.txt", std::ofstream::binary);

	ecb.decrypt(input, output);
	
	return;
}