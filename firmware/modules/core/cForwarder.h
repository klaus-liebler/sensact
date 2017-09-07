#pragma once
#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact{



	class cForwarder: public cApplication{
	private:
		eApplicationID finalTarget;
	public:
		eAppResult Setup() override;
		eAppType GetAppType() override;
#include <ForwarderApplication.hinc>


		cForwarder(eApplicationID id, eApplicationID finalTarget);
	};



}



