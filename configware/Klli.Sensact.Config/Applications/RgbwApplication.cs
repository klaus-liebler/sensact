﻿using Klli.Sensact.Config.Nodes;
using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class RGBW
    {
        public int R;
        public int G;
        public int B;
        public int W;
    }

    public class RgbwApplication : ActorApplication
    {
        public PwmPin OutputR;
        public PwmPin OutputG;
        public PwmPin OutputB;
        public PwmPin OutputW;
        public bool LowMeansLampOn;
        public string StandbyController;

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            HashSet<CommandType> ret = new HashSet<CommandType>()
            {
                CommandType.SET_RGBW, 
                CommandType.SET_SIGNAL,
            };
            return ret;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string GenerateInitializer(Model m)
        {

            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// RGBW {0} (Full Color Light)" + Environment.NewLine, ApplicationId);
            //sensact::cRgbw BATH("RGBWBATH", eApplicationID::BELL__DOOR, ePWMOutput::O1_01, ePWMOutput::O1_02, ePWMOutput::O1_03, ePWMOutput::NONE, false, (uint8_t*)MODEL::wellKnownRGBWColors, 2, eApplicationID::STDBY_XX_XXX_1);
            sb.AppendFormat("sensact::cRgbw {0}(\"{0}\", eApplicationID::{0}, ePWMOutput::{1}, ePWMOutput::{2}, ePWMOutput::{3}, ePWMOutput::{4}, {5}, (uint8_t*)MODEL::wellKnownRGBWColors, MODEL::wellKnownRGBWColorsCnt, eApplicationID::{6});" + Environment.NewLine + Environment.NewLine, ApplicationId, OutputR, OutputG, OutputB, OutputW, LowMeansLampOn.ToString().ToLower(), StandbyController!=null?StandbyController:"NO_APPLICATION");
            return sb.ToString();
        }

        //PWM PushButton
        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("PWM"+REGEX_FLOOR_ROOM_SUFFIX);
            }
        }
    }
}