#pragma once
#include<cstdint>
/*
* abstract class for input and output
*/
class IIO
{
public:
	virtual uint64_t read() = 0;
	virtual void write(uint64_t& data) = 0;
};

