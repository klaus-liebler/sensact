#pragma once

#include <vector>
#include <array>
#include <cstring>

namespace sensact::model::applications
{
	constexpr auto ApplicationNames =std::to_array({
		#include "applicationNames.inc"
	});
	constexpr auto CommandNames =std::to_array({
		#include "commandNames.inc"
	});
	constexpr auto EventNames =std::to_array({
		#include "eventNames.inc"
	});
};