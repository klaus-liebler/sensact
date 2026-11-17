#pragma once
#include <cApplication.hpp>

using namespace sensactcore;
namespace sensactapps {
	class cForwarder: public cApplication{
	private:
		eApplicationID finalTarget;
	public:
		eAppCallResult Setup(SensactContext *ctx) override;
		eAppType GetAppType() override;
#include <application/ForwarderApplication.inc>


		cForwarder(eApplicationID id, eApplicationID finalTarget);
	};



}



