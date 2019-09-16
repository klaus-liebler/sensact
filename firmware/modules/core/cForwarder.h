#pragma once
#include "common.h"
#include "cBsp.h"
#include "../generated/cApplication.h"

namespace sensact{



	class cForwarder: public cApplication{
	private:
		eApplicationID finalTarget;
	public:
		eAppCallResult Setup() override;
		eAppType GetAppType() override;
#include "../generated/ForwarderApplication.hinc"


		cForwarder(eApplicationID id, eApplicationID finalTarget);
	};



}



