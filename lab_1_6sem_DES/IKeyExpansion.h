#pragma once
#include<vector>
class IKeyExpansion
{
public:
	virtual std::vector<uint64_t> generateRoundKey(int iteration, const uint8_t* key
		, const std::vector<uint8_t>& keyShiftArr, const std::vector<uint8_t>& keyPermArr) = 0;
	virtual ~IKeyExpansion() = default;
};

