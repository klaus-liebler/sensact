#include <algorithm> // std::max
#include "aSinglePwm.hh"
#define TAG "SINGLEPWM"

namespace sensactapps
{

	cSinglePWM::cSinglePWM(uint32_t id, const uint16_t pwmFirst, const uint16_t pwmLast, uint32_t autoOffMsecs, uint32_t idOfStandbyController) : cApplication(id), pwmFirst(pwmFirst), pwmLast(pwmLast), autoOffMsecs(autoOffMsecs), idOfStandbyController(idOfStandbyController), intensity_0_1(0.5), on(false), changed(false)
	{
		LOGI(TAG, "Build cSinglePWM for id:%d, pwmFirst:%d autoOffMsecs:%d idOfStandbyController:%d", id, pwmFirst, autoOffMsecs, idOfStandbyController);
	}

	ErrorCode cSinglePWM::Setup(SensactContext *ctx)
	{
		return ErrorCode::OK;
	}

	ErrorCode cSinglePWM::Loop(SensactContext *ctx)
	{
		if (!changed && on && autoOffMsecs != 0 && lastChanged + autoOffMsecs < ctx->now)
		{
			// das !changed muss auch abgefragt werden, weil sonst ein Command hier direkt abgeblockt werden könnte
			LOGI(TAG, "AutoOff id %d after %d ms", id, autoOffMsecs);
			this->changed = true;
			this->on = false;
		}

		if (idOfStandbyController != 0 && this->on && lastHeartbeatSent + 3000 < ctx->now)
		{
			LOGI(TAG, "Sending heartbeat from id %d to id %d", id, idOfStandbyController);
			flatbuffers::FlatBufferBuilder builder(64);
			auto onOffCmd = CreatetOnOffCommand(builder, eOnOffCommand_TRIGGER);
			auto cmd = sensact::comm::CreatetCommand(builder, idOfStandbyController, uCommand::uCommand_tOnOffCommand, onOffCmd.Union());
			builder.Finish(cmd);
			const tCommand *command_parsed = flatbuffers::GetRoot<tCommand>(builder.GetBufferPointer());
			ctx->postOffice->PostCommand(command_parsed);
			lastHeartbeatSent = ctx->now;
		}
		if (!this->changed)
		{
			return ErrorCode::OK;
		}
		LOGI(TAG, "Changes occuredin id %d", id);
		this->changed = false;
		this->lastChanged = ctx->now;
		if (!this->on)
		{
			for (int out = pwmFirst; out <= pwmLast; out++)
			{
				ctx->io->SetU16Output(out, 0);
			}
			return ErrorCode::OK;
		}
		float val = this->intensity_0_1 > 0.99 ? UINT16_MAX : this->intensity_0_1 * (float)UINT16_MAX;
		for (int out = pwmFirst; out <= pwmLast; out++)
		{
			ctx->io->SetU16Output(out, val);
		}
		return ErrorCode::OK;
	}

	ErrorCode cSinglePWM::FillStatus(flatbuffers::FlatBufferBuilder *builder, std::vector<flatbuffers::Offset<tStateWrapper>> *status_vector)
	{
		auto x = CreatetSinglePwmState(*builder, this->intensity_0_1, this->on);
		auto state = CreatetStateWrapper(*builder, this->id, uState::uState_tSinglePwmState, x.Union());
		status_vector->push_back(state);
		return ErrorCode::OK;
	}

	ErrorCode cSinglePWM::ProcessCommand(const tCommand *msg)
	{
		if (msg->command_type() != uCommand::uCommand_tSinglePwmCommand)
		{
			return ErrorCode::INVALID_COMMAND;
		}
		auto cmd = msg->command_as_tSinglePwmCommand();
		switch (cmd->cmd())
		{
		case eSinglePwmCommand_TOGGLE:
			this->on = !this->on;
			this->changed = true;
			LOGI(TAG, "eSinglePwmCommand_TOGGLE for id %d, now %s", id, on ? "on" : "off");
			break;
		case eSinglePwmCommand_ON:
			if (!this->on)
			{
				on = true;
				changed = true;
				LOGI(TAG, "eSinglePwmCommand_ON for id %d, now %s", id, on ? "on" : "off");
			}
			break;
		case eSinglePwmCommand_OFF:
			if (this->on)
			{
				on = false;
				changed = true;
				LOGI(TAG, "eSinglePwmCommand_OFF for id %d, now %s", id, on ? "on" : "off");
			}
			break;
		case eSinglePwmCommand_CHANGE_INTENSITY:
			this->intensity_0_1 = cmd->intensity_0_1();
			this->changed = true;
			LOGI(TAG, "eSinglePwmCommand_CHANGE_INTENSITY for id %d, now %f", id, intensity_0_1);
			break;
		default:
			return ErrorCode::INVALID_COMMAND;
		}
		return ErrorCode::OK;
	}

	cSinglePWM *cSinglePWM::Build(uint32_t const id, const tConfigWrapper *cfg)
	{
		if (cfg->config_type() != uConfig::uConfig_tSinglePwmConfig)
		{
			return nullptr;
		}
		auto x = cfg->config_as_tSinglePwmConfig();
		return new cSinglePWM(id, x->pwmFirst(), x->pwmLast(), x->autoOffMsecs(), x->idOfStandbyController());
	}
}