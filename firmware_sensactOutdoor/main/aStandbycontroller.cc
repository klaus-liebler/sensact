#include <algorithm>    // std::max
#include "aStandbycontroller.hh"


cStandbyController::cStandbyController(uint32_t const id, uint16_t const relay, uint32_t const waitTimeMsecs):
	cApplication(id), relay(relay), waitTimeMsecs(waitTimeMsecs), state(false), lastHeartbeat(0), triggered(false){
}


ErrorCode cStandbyController::Setup(SensactContext *ctx)
{
	return ctx->node->SetU16Output(this->relay, INACTIVE);
}

ErrorCode cStandbyController::Loop(SensactContext *ctx)
{
	if(triggered){
		lastHeartbeat=ctx->now;
		triggered=false;
	}
	if(ctx->now-lastHeartbeat > waitTimeMsecs && this->state==true)
	{
		ctx->node->SetU16Output(relay, INACTIVE);
		this->state=false;
	}else if(ctx->now-lastHeartbeat <= waitTimeMsecs && this->state==false){
		ctx->node->SetU16Output(relay, ACTIVE);
		this->state=true;
	}
	return ErrorCode::OK;
}

ErrorCode cStandbyController::FillStatus(BinaryWriter *w){
	w->StartWriting(this->id, 1);
	w->WriteU32((uint32_t)this->state);
	w->EndWriting(this->id);
	return ErrorCode::OK;
}

ErrorCode cStandbyController::ProcessJsonCommand(cJSON * json){
	this->triggered=true;
	return ErrorCode::OK;
}
cStandbyController* cStandbyController::BuildFromJSON(const uint32_t id, cJSON * json){
	uint16_t relay = cJSON_GetObjectItem(json, "relay")->valueint;
	uint16_t waitTimeMsecs = cJSON_GetObjectItem(json, "waitTimeMsecs")->valueint;
	return new cStandbyController(id, relay, waitTimeMsecs);
}
