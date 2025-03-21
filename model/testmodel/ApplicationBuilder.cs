﻿using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using System.Collections.Generic;
using System.Linq;
using Klli.Sensact.Model.Common;

namespace Klli.Sensact.Model
{
    public static class AB
    {
        public const int DEFAULT_MIN_DIM_LEVEL = 2;

        public static void AddPlaceholder(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription)
        {
            list.Add(new PlaceholderApplication((ushort)appId, appId.ToString(), ApplicationDescription));
        }

        public static void AddBlindApplication(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription, ushort Relay1, ushort Relay2, RelayInterlockMode relMode, uint time_up_secs, uint time_down_secs)
        {
            list.Add(new BlindApplication((ushort)appId, appId.ToString(), ApplicationDescription, Relay1, Relay2, relMode, time_up_secs, time_down_secs));
        }


        public static void AddPWMApplication(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription, ApplicationId StandbyController, ISet<ushort> outputResources, uint AutoOffIntervalMsecs = 0, byte minimalOnLevel=DEFAULT_MIN_DIM_LEVEL, byte initialStoredTargetLevel=byte.MaxValue)
        {
            list.Add(new SinglePWMApplication((ushort)appId, appId.ToString(), ApplicationDescription, outputResources, minimalOnLevel, initialStoredTargetLevel, AutoOffIntervalMsecs, (ushort)StandbyController));
        }


        public static void AddToggleButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ISet<ApplicationId> targetAppIds)
        {
            list.Add(new PushButtonSingle2ToggleApplication((ushort)appId, appId.ToString(), inputRessource, targetAppIds.Select(x=>(ushort)x).ToHashSet()));
        }

        public static void AddToggleButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(new PushButtonSingle2ToggleApplication((ushort)appId, appId.ToString(), inputRessource, new HashSet<ushort>{(ushort)targetAppId}));
        }

        public static void AddPowIt(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription,  ushort outputRessource, uint autoOffIntervalMsecs = 0, PowerState initialPowerState=PowerState.INACTIVE)
        {
            list.Add(new OnOffApplication((ushort)appId, appId.ToString(), ApplicationDescription, outputRessource, initialPowerState, autoOffIntervalMsecs));
        }

        public static void AddSound(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription, byte initialVolume=100, ushort defaultSignalOnToggle=1){
            list.Add(new SoundApplication((ushort)appId, appId.ToString(), ApplicationDescription, initialVolume, defaultSignalOnToggle));

        }

        public static void AddFingerprint(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription, ApplicationId targetAppId1, ApplicationId targetAppId2, ApplicationId targetAppId3){
            list.Add(new FingerprintApplication((ushort)appId, appId.ToString(), ApplicationDescription, (ushort)targetAppId1, (ushort)targetAppId2, (ushort)targetAppId3));

        }
        public static void AddMilightControllerApplication(this List<SensactApplication> list, ApplicationId appId, string ApplicationDescription){
            list.Add(new MilightControllerApplication((ushort)appId, appId.ToString(), ApplicationDescription));
        }


        public static void AddRotaryEncoder(this List<SensactApplication> list, ApplicationId appId, RotaryEncoder rotEnc, ApplicationId targetAppId)
        {
            list.Add(BuildRotaryEncoder(appId, rotEnc, targetAppId));
        }
        public static RotaryEncoder2PWMApplication BuildRotaryEncoder(ApplicationId appId, RotaryEncoder rotEnc, ApplicationId targetAppId)
        {
            return new RotaryEncoder2PWMApplication((ushort)appId, appId.ToString(), (ushort)rotEnc, (ushort)targetAppId);
        }

        public static RotaryEncoder2PWMApplication BuildRotaryEncoder(ushort appId, RotaryEncoder rotEnc, ushort targetAppId)
        {
            return new RotaryEncoder2PWMApplication((ushort)appId, appId.ToString(), (ushort)rotEnc, (ushort)targetAppId);
        }
        
        public static void AddOneDimButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(BuildOnePushbuttonDimmer(appId, inputRessource, targetAppId));
        }
        

        public static PushButtonSingleApplication BuildOnePushbuttonDimmer(ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            return new PushButtonSingle2PwmSingleApplication((ushort)appId, appId.ToString(), inputRessource, (ushort)targetAppId);
        }

        public static void AddTwoDimButtons(this List<SensactApplication> list, ApplicationId appIdUp, ushort inputPinUp, ushort inputPinDown, ApplicationId targetAppId)
        {
            list.Add(new PushButtonDual2PWMApplication((ushort)appIdUp, appIdUp.ToString(), inputPinUp, inputPinDown, (ushort)targetAppId));
        }

        public static void AddBlindButtons(this List<SensactApplication> list, ApplicationId appIdUp, ushort inputPinUp, ushort inputPinDown, ApplicationId targetAppId)
        {
            list.Add(new PushButtonDual2BlindApplication((ushort)appIdUp, appIdUp.ToString(), inputPinUp, inputPinDown, (ushort)targetAppId));
        }
    }
}
