#pragma once
#include <application.hh>

namespace sensact::apps
{
	class cPushbutton
	{
	private:
		InOutId const input;

		bool isPressedOld{false};
		bool holdShortSent{false};

		tms_t lastPress;
		tms_t lastRelease;

	protected:
		cPushbutton(InOutId const input) : input(input)
		{
		}
		virtual void OnPressed(SensactContext *ctx){};
		virtual void OnReleasedShort(SensactContext *ctx){};
		virtual void OnReleasedLong(SensactContext *ctx){};
		virtual void OnReleased(SensactContext *ctx){};
		virtual void OnDoubleclick(SensactContext *ctx){};
		virtual void OnPressedShortAndHold(SensactContext *ctx){};

	public:
		void ButtonLoop(SensactContext *ctx)
		{
			u16 inputValue;

			tms_t now = ctx->Now();
			ctx->GetU16Input(input, inputValue);
			bool isPressed = inputValue == 0; // because all buttons are connected to GND
			if (!isPressedOld && isPressed)
			{
				this->holdShortSent = false;
				OnPressed(ctx);
				this->lastPress = now;
			}
			else if (isPressedOld && !isPressed)
			{
				if (now - this->lastPress < sensact::magic::SHORT_PRESS)
				{
					OnReleasedShort(ctx);
				}
				else
				{
					OnReleasedLong(ctx);
				}

				OnReleased(ctx);

				if (now - lastRelease < sensact::magic::DOUBLE_PRESS)
				{
					OnDoubleclick(ctx);
				}

				this->lastRelease = now;
			}
			else if (isPressedOld && isPressed)
			{
				if (!this->holdShortSent && now - lastPress >= sensact::magic::SHORT_PRESS)
				{
					OnPressedShortAndHold(ctx);
					this->holdShortSent = true;
				}
			}
			this->isPressedOld = isPressed;
		}
	};

	class cPushbuttonSingle2OnOff : public cApplication, public cPushbutton
	{
	private:
		eApplicationID target;

	protected:
		void OnPressed(SensactContext *ctx) override { ctx->SendONCommand(target, 0); }

	public:
		cPushbuttonSingle2OnOff(eApplicationID const id, InOutId const input, eApplicationID target) : cApplication(id), cPushbutton(input), target(target)
		{
		}

		eAppCallResult Loop(SensactContext *ctx) override
		{
			this->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}
	};

	class cPushbuttonSingle2PwmSingle : public cApplication, public cPushbutton
	{
	private:
		eApplicationID target;

	protected:
		void OnReleasedShort(SensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
		void OnReleasedLong(SensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		void OnPressedShortAndHold(SensactContext *ctx) override { ctx->SendSTARTCommand(target); }
		void OnDoubleclick(SensactContext *ctx) override { ctx->SendONCommand(target, 0); }

	public:
		cPushbuttonSingle2PwmSingle(eApplicationID const id, InOutId const input, eApplicationID target) : cApplication(id), cPushbutton(input), target(target)
		{
		}

		eAppCallResult Loop(SensactContext *ctx) override
		{
			this->ButtonLoop(ctx);
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
			void OnReleasedShort(SensactContext *ctx) override { ctx->SendUPCommand(target, 0); }
			void OnPressedShortAndHold(SensactContext *ctx) override { ctx->SendUPCommand(target, 1); }
			void OnReleasedLong(SensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

		class cPushbuttonDOWN : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonDOWN(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(SensactContext *ctx) override { ctx->SendDOWNCommand(target, 0); }
			void OnPressedShortAndHold(SensactContext *ctx) override { ctx->SendDOWNCommand(target, 1); }
			void OnReleasedLong(SensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

	private:
		cPushbuttonUP *up;
		cPushbuttonDOWN *down;

	public:
		cPushbuttonDual2Blind(eApplicationID const id, InOutId const inputUp, InOutId const inputDown, eApplicationID target) : cApplication(id)
		{
			up = new cPushbuttonUP(inputUp, target);
			down = new cPushbuttonDOWN(inputUp, target);
		}

		eAppCallResult Loop(SensactContext *ctx) override
		{
			down->ButtonLoop(ctx);
			up->ButtonLoop(ctx);
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
			void OnReleasedShort(SensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
			void OnPressedShortAndHold(SensactContext *ctx) override { ctx->SendUPCommand(target, 0); }
			void OnReleasedLong(SensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

		class cPushbuttonDOWN : public cPushbutton
		{
		private:
			eApplicationID target;

		public:
			cPushbuttonDOWN(InOutId const input, eApplicationID target) : cPushbutton(input), target(target) {}
			void OnReleasedShort(SensactContext *ctx) override { ctx->SendTOGGLECommand(target); }
			void OnPressedShortAndHold(SensactContext *ctx) override { ctx->SendDOWNCommand(target, 0); }
			void OnReleasedLong(SensactContext *ctx) override { ctx->SendSTOPCommand(target); }
		};

	private:
		cPushbuttonUP *up;
		cPushbuttonDOWN *down;

	public:
		cPushbuttonDual2PWM(eApplicationID const id, InOutId const inputUp, InOutId const inputDown, eApplicationID target) : cApplication(id)
		{
			up = new cPushbuttonUP(inputUp, target);
			down = new cPushbuttonDOWN(inputUp, target);
		}

		eAppType GetAppType() override
		{
			return eAppType::PSHBT;
		}

		eAppCallResult Loop(SensactContext *ctx) override
		{
			down->ButtonLoop(ctx);
			up->ButtonLoop(ctx);
			return eAppCallResult::OK;
		}
	};
}