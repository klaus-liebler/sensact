#pragma once

#include <vector>
#include <array>
#include <cstring>

namespace sensact::model::applications
{
	constexpr auto ApplicationNames =std::to_array({
		#include "applicationNames.inc"
	});
};