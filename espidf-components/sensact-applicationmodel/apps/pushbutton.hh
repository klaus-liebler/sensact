#pragma once
#include <vector>
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
		// beim Booten schon gedrueckt (oder klemmend): bis zum Loslassen keine Ereignisse
		bool ignoreUntilReleased{false};

		tms_t lastPress{0};
		tms_t lastRelease{INT64_MIN / 2}; // "lange her" -- kein Doppelklick direkt nach dem Booten
		static constexpr const char *PUSHB_TAG = "PUSHB";

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

		// Startzustand einlesen: ein beim Booten bereits gedrueckter oder klemmender Taster darf
		// kein Ereignis ausloesen (frueher: TOGGLE, beim Rollladen-Taster nach 400 ms sogar
		// Dauerfahrt).
		void ButtonSetup(iSensactContext *ctx)
		{
			u16 inputValue;
			ctx->GetU16Input(input, inputValue);
			this->isPressedOld = inputValue != 0;
			this->ignoreUntilReleased = this->isPressedOld;
			this->holdShortSent = this->isPressedOld;
			if (this->isPressedOld)
			{
				LOGW(PUSHB_TAG, "input %u is active at startup -- ignored until released", (unsigned)input);
			}
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
				LOGI(PUSHB_TAG, "OnPressed");
				OnPressed(ctx);
				this->lastPress = now;
			}
			else if (isPressedOld && !isPressed && ignoreUntilReleased)
			{
				this->ignoreUntilReleased = false;
			}
			else if (isPressedOld && !isPressed)
			{
				if (now - this->lastPress < sensact::magic::SHORT_PRESS)
				{
					LOGI(PUSHB_TAG, "OnReleasedShort");
					OnReleasedShort(ctx);
				}
				else
				{
					LOGI(PUSHB_TAG, "OnReleasedLong");
					OnReleasedLong(ctx);
				}
				LOGI(PUSHB_TAG, "OnReleased");
				OnReleased(ctx);

				if (now - lastRelease < sensact::magic::DOUBLE_PRESS)
				{
					LOGI(PUSHB_TAG, "OnDoubleclick");
					OnDoubleclick(ctx);
				}

				this->lastRelease = now;
			}
			else if (isPressedOld && isPressed)
			{
				if (!this->holdShortSent && now - lastPress >= sensact::magic::SHORT_PRESS)
				{
					LOGI(PUSHB_TAG, "OnPressedShortAndHold");
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
			this->ButtonSetup(ctx);
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=IsPressedNow();
			buf[2]=0;
			return eFillStatusResult::OK;
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
			this->ButtonSetup(ctx);
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=IsPressedNow();
			buf[2]=0;
			return eFillStatusResult::OK;
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
			up->ButtonSetup(ctx);
			down->ButtonSetup(ctx);
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=down->IsPressedNow();
			buf[2]=up->IsPressedNow();
			return eFillStatusResult::OK;
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
			up->ButtonSetup(ctx);
			down->ButtonSetup(ctx);
			return eAppCallResult::OK;
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eFillStatusResult FillStatus(iSensactContext &ctx, std::array<uint16_t, 4>& buf) override{
			buf[0]=buf[3]=0;
			buf[1]=down->IsPressedNow();
			buf[2]=up->IsPressedNow();
			return eFillStatusResult::OK;
		}
	};
}