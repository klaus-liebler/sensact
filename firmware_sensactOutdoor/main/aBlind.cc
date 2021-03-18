#include "aBlind.hh"

constexpr uint16_t WAITTIME = 1000;
constexpr uint16_t OPEN_CLOSE_TIME = 60000;


cBlind::cBlind(eApplicationID const id, uint16_t const relayUp, uint16_t const relayDown):
	cApplication(id), relayUp(relayUp), relayDown(relayDown), lastChanged(0L), currentState(eBlindState::STOP), requestedState(eBlindState::STOP)
{
}

ErrorCode cBlind::Setup(SensactContext *ctx) {
	return ErrorCode::OK;
}


void cBlind::up(SensactContext *ctx)
{
	this->currentState=eBlindState::UP;
	this->lastChanged=ctx->now;
	ctx->node->SetU16Output(relayUp, ACTIVE);
	ctx->node->SetU16Output(relayDown, INACTIVE);		
}

void cBlind::down(SensactContext *ctx)
{
	this->currentState=eBlindState::DOWN;
	this->lastChanged=ctx->now;
	ctx->node->SetU16Output(relayUp, INACTIVE);
	ctx->node->SetU16Output(relayDown, ACTIVE);	
}
void cBlind::stop(SensactContext *ctx){
	this->currentState=eBlindState::STOP;
	this->lastChanged=ctx->now;
	ctx->node->SetU16Output(relayUp, INACTIVE);
	ctx->node->SetU16Output(relayDown, INACTIVE);
}

void cBlind::UpPressed()
{
	this->requestedState=eBlindState::UP;
}

void cBlind::StopPressed()
{
	this->requestedState=eBlindState::STOP;
}

void cBlind::DownPressed()
{
	this->requestedState=eBlindState::DOWN;
}

ErrorCode cBlind::Loop(SensactContext *ctx)
{
	if((this->currentState==eBlindState::UP || this->currentState==eBlindState::DOWN) && ctx->now-lastChanged >= OPEN_CLOSE_TIME){
		this->requestedState=eBlindState::STOP;
	}
	if(this->currentState==this->requestedState){
		return ErrorCode::OK;
	}
	if(this->currentState==eBlindState::STOP && ctx->now-lastChanged >= WAITTIME){
		if(this->requestedState==eBlindState::DOWN){
			down(ctx);
		} else if(this->requestedState==eBlindState::UP){
			up(ctx);
		}else{//should never happen!
			stop(ctx);
		}
		return ErrorCode::OK;
	}
	stop(ctx);
	return ErrorCode::OK;
}


