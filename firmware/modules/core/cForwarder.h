#pragma once
#include "common.h"
#include "cBsp.h"
#include "cApplication.h"

namespace sensact{



	class cForwarder: public cApplication{
	private:
		eApplicationID finalTarget;
	public:
		bool Setup() override;
#include <ForwarderApplication.hinc>


		cForwarder(const char* name, eApplicationID id, eApplicationID finalTarget);
	};



}



