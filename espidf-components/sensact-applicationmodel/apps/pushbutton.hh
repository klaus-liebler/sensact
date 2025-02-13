#pragma once
#define TAG "PUSHB"
#include <sensact_logger.hh>
#include "cApplication.hh"

namespace sensact::apps
{
	class cPushbutton
	{
	private:
		InOutId const input;

		bool isPressedOld{false};
		bool holdShortSent{false};

		tms_t lastPress{0};
		tms_t lastRelease{0};

	protected:
		cPushbutton(InOutId const input) : input(input)
		{
		}
		virtual void OnPressed(iSensactContext *ctx){};
		virtual void OnReleasedShort(iSensactContext *ctx){};
		virtual void OnReleasedLong(iSensactContext *ctx){};
		virtual void OnReleased(iSensactContext *ctx){};
		virtual void OnDoubleclick(iSensactContext *ctx){};
		virtual void OnPressedShortAndHold(iSensactContext *ctx){};

		

	public:

		bool IsPressedNow()
		{
			return isPressedOld;
		}
		void ButtonLoop(iSensactContext *ctx)
		{
			u16 inputValue;
			tms_t now = ctx->Now();
			ctx->GetU16Input(input, inputValue);
			bool isPressed = inputValue != 0;
			if (!isPressedOld && isPressed)
			{
				this->holdShortSent = false;
				LOGI(TAG, "OnPressed");
				OnPressed(ctx);
				this->lastPress = now;
			}
			else if (isPressedOld && !isPressed)
			{
				if (now - this->lastPress < sensact::magic::SHORT_PRESS)
				{
					LOGI(TAG, "OnReleasedShort");
					OnReleasedShort(ctx);
				}
				else
				{
					LOGI(TAG, "OnReleasedLong");
					OnReleasedLong(ctx);
				}
				LOGI(TAG, "OnReleased");
				OnReleased(ctx);

				if (now - lastRelease < sensact::magic::DOUBLE_PRESS)
				{
					LOGI(TAG, "OnDoubleclick");
					OnDoubleclick(ctx);
				}

				this->lastRelease = now;
			}
			else if (isPressedOld && isPressed)
			{
				if (!this->holdShortSent && now - lastPress >= sensact::magic::SHORT_PRESS)
				{
					LOGI(TAG, "OnPressedShortAndHold");
					OnPressedShortAndHold(ctx);
					this->holdShortSent = true;
				}
			}
			this->isPressedOld = isPressed;
		}
	};

	class cPushbuttonSingle2Toggle : public cApplication, public cPushbutton
	{
	private:
		std::vector<eApplicationID> targets;

	protected:
		void OnPressed(iSensactContext *ctx) override
		{
			for (auto &target : targets)
			{
				ctx->SendTOGGLECommand(target);
			}
		}

	public:
		cPushbuttonSingle2Toggle(eApplicationID const id, InOutId const input, std::vector<eApplicationID> targets) : cApplication(id), cPushbutton(input), targets(targets)
		{
		}

		eAppCallResult Loop(iSensactContext *ctx) override
		{
			this->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}

		eAppCallResult Setup(iSensactContext *ctx) override
		{
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=IsPressedNow();
			buf[2]=0;
			return eAppCallResult::OK;
		}
	};

	class cPushbuttonSingle2PwmSingle : public cApplication, public cPushbutton
	{
	private:
		eApplicationID target;

	protected:
		void OnReleasedShort(iSensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
		void OnReleasedLong(iSensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		void OnPressedShortAndHold(iSensactContext *ctx) override { ctx->SendSTARTCommand(target); }
		void OnDoubleclick(iSensactContext *ctx) override { ctx->SendONCommand(target, 0); }

	public:
		cPushbuttonSingle2PwmSingle(eApplicationID const id, InOutId const input, eApplicationID target) : cApplication(id), cPushbutton(input), target(target)
		{
		}

		eAppCallResult Loop(iSensactContext *ctx) override
		{
			this->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}

		eAppCallResult Setup(iSensactContext *ctx) override
		{
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=IsPressedNow();
			buf[2]=0;
			return eAppCallResult::OK;
		}
	};

	class cPushbuttonDual2Blind : public cApplication
	{
		class cPushbuttonUP : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonUP(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(iSensactContext *ctx) override { ctx->SendUPCommand(target, 0); }
			void OnPressedShortAndHold(iSensactContext *ctx) override { ctx->SendUPCommand(target, 1); }
			void OnReleasedLong(iSensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

		class cPushbuttonDOWN : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonDOWN(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(iSensactContext *ctx) override { ctx->SendDOWNCommand(target, 0); }
			void OnPressedShortAndHold(iSensactContext *ctx) override { ctx->SendDOWNCommand(target, 1); }
			void OnReleasedLong(iSensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

	private:
		cPushbuttonUP *up;
		cPushbuttonDOWN *down;

	public:
		cPushbuttonDual2Blind(eApplicationID const id, InOutId const inputUp, InOutId const inputDown, eApplicationID target) : cApplication(id)
		{
			up = new cPushbuttonUP(inputUp, target);
			down = new cPushbuttonDOWN(inputDown, target);
		}

		eAppCallResult Loop(iSensactContext *ctx) override
		{
			down->ButtonLoop(ctx);
			up->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}

		eAppCallResult Setup(iSensactContext *ctx) override
		{
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=down->IsPressedNow();
			buf[2]=up->IsPressedNow();
			return eAppCallResult::OK;
			return eAppCallResult::OK;
		}
	};

	class cPushbuttonDual2PWM : public cApplication
	{
		class cPushbuttonUP : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonUP(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(iSensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
			void OnPressedShortAndHold(iSensactContext *ctx) override { ctx->SendUPCommand(target, 0); }
			void OnReleasedLong(iSensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

		class cPushbuttonDOWN : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonDOWN(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(iSensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
			void OnPressedShortAndHold(iSensactContext *ctx) override { ctx->SendDOWNCommand(target, 0); }
			void OnReleasedLong(iSensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

	private:
		cPushbuttonUP *up;
		cPushbuttonDOWN *down;

	public:
		cPushbuttonDual2PWM(eApplicationID const id, InOutId const inputUp, InOutId const inputDown, eApplicationID target) : cApplication(id)
		{
			up = new cPushbuttonUP(inputUp, target);
			down = new cPushbuttonDOWN(inputDown, target);
		}

		eAppCallResult Loop(iSensactContext *ctx) override
		{
			down->ButtonLoop(ctx);
			up->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}

		eAppCallResult Setup(iSensactContext *ctx) override
		{
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eAppCallResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=down->IsPressedNow();
			buf[2]=up->IsPressedNow();
			return eAppCallResult::OK;
		}
	};
}
#undef TAG