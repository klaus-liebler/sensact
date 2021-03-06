using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class AB
    {
        public const int DEFAULT_MIN_DIM_LEVEL = 2;

        public static void AddPlaceholder(this List<SensactApplication> list, ApplicationId appId)
        {
            list.Add(new PlaceholderApplication()
            {
                ApplicationId = appId.ToString(),
            });
        }

        public static void AddBlindApplication(this List<SensactApplication> list, ApplicationId appId, int OpenCloseTimeInSeconds, ushort OutputResourcePower, ushort OutputResourceDirection, RelayMode relMode)
        {
            list.Add(new BlindApplication { ApplicationId = appId.ToString(), OpenCloseTimeInSeconds= OpenCloseTimeInSeconds, OutputResourceUpOrPower=OutputResourcePower, OutputResourceDownOrDirection=OutputResourceDirection, RelayMode=relMode });
        }

        public static void AddPWMApplication(this List<SensactApplication> list, ApplicationId appId, ApplicationId standbyController, List<ushort> outputResources, int AutoOffIntervalMsecs= 0, int minimalOnLevel=DEFAULT_MIN_DIM_LEVEL)
        {
            list.Add(new PWMApplication() { ApplicationId = appId.ToString(), MinimalOnLevel = minimalOnLevel, StandbyController = standbyController.ToString(), OutputRessources = outputResources, AutoOffIntervalMsecs= AutoOffIntervalMsecs });
        }

        public static void AddToggleButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(new PushButtonXApplication()
            {
                ApplicationId = appId.ToString(),
                InputRessource = inputRessource,
                CommandsOnPressed = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
            });
        }

        public static void AddOnIfDarkButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(new PushButtonXApplication()
            {
                ApplicationId = appId.ToString(),
                InputRessource = inputRessource,
                CommandsOnPressed = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
            });
        }


        public static void AddPowIt(this List<SensactApplication> list, ApplicationId appId, ushort outputRessource, uint autoOffIntervalMsecs = 0, uint autoOnIntervalMsecs = 0)
        {
            list.Add(new PoweritemApplication()
            {
                ApplicationId = appId.ToString(),
                OutputRessource = outputRessource,
                AutoOffIntervalMsecs= autoOffIntervalMsecs,
                AutoOnIntervalMsecs= autoOnIntervalMsecs
            });
        }

        public static void AddRotaryEncoder(this List<SensactApplication> list, ApplicationId appId, RotaryEncoder rotEnc, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(BuildRotaryEncoder(appId, rotEnc, inputRessource, targetAppId));
        }
        public static RotaryEncoderApplication BuildRotaryEncoder(ApplicationId appId, RotaryEncoder rotEnc, ushort inputRessource, ApplicationId targetAppId)
        {
            return new RotaryEncoderApplication
            {
                ApplicationId = appId.ToString(),
                InputRotaryRessource = rotEnc,
                InputPushRessource = inputRessource,
                CommandsOnPressed = new List<Command>
                {
                    new Command
                    {
                        TargetAppId=targetAppId.ToString(),
                        CommandType=CommandType.TOGGLE,
                    }
                },
                CommandsOnTurned = new List<Command>
                {
                    new Command
                    {
                        TargetAppId=targetAppId.ToString(),
                        CommandType=CommandType.STEP_VERTICAL,
                    }
                }
            };
        }
        public static void AddOneDimButton(this List<SensactApplication> list, ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            list.Add(BuildOnePushbuttonDimmer(appId, inputRessource, targetAppId));
        }
        

        public static PushButtonXApplication BuildOnePushbuttonDimmer(ApplicationId appId, ushort inputRessource, ApplicationId targetAppId)
        {
            return new PushButtonXApplication()
            {
                ApplicationId = appId.ToString(),
                InputRessource = inputRessource,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=targetAppId.ToString(),
                    },
                },

                CommandsOnPressedShortAndHold = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.START,
                        TargetAppId=targetAppId.ToString(),
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
                CommandsOnDoubleclick = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.ON,
                        TargetAppId=targetAppId.ToString(),
                    },
                },

            };
        }

        public static void AddTwoDimButtons(this List<SensactApplication> list, ApplicationId appIdUp, ApplicationId appIdDown, ushort inputPinUp, ushort inputPinDown, ApplicationId targetAppId)
        {
            PushButtonXApplication up = new PushButtonXApplication
            {
                ApplicationId = appIdUp.ToString(),
                InputRessource = inputPinUp,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=targetAppId.ToString(),
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
                CommandsOnPressedShortAndHold = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.UP,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
            };
            PushButtonXApplication down = new PushButtonXApplication
            {
                ApplicationId = appIdDown.ToString(),
                InputRessource = inputPinDown,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId=targetAppId.ToString(),
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
                CommandsOnPressedShortAndHold = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.DOWN,
                        TargetAppId=targetAppId.ToString(),
                    },
                },
            };
            list.Add(up);
            list.Add(down);
        }

        public static void AddBlindButtons(this List<SensactApplication> list, ApplicationId appIdUp, ApplicationId appIdDown, ushort inputPinUp, ushort inputPinDown, ApplicationId targetAppId)
        {
            list.AddBlindButtons(appIdUp.ToString(), appIdDown.ToString(), inputPinUp, inputPinDown, targetAppId.ToString());
        }

        public static void AddBlindButtons(this List<SensactApplication> list, string appIdUp, string appIdDown, ushort inputPinUp, ushort inputPinDown, string targetAppId)
        {
            PushButtonXApplication up = new PushButtonXApplication
            {
                ApplicationId = appIdUp,
                InputRessource = inputPinUp,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.UP,
                        TargetAppId=targetAppId,
                        Payload=new byte[]{0},
                    },
                },
                CommandsOnPressedShortAndHold= new List<Command>()
                {
                     new Command()
                    {
                        CommandType=CommandType.UP,
                        TargetAppId=targetAppId,
                        Payload=new byte[]{1},
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId=targetAppId,
                    },
                },
            };
            PushButtonXApplication down = new PushButtonXApplication
            {
                ApplicationId = appIdDown,
                InputRessource = inputPinDown,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.DOWN,
                        TargetAppId=targetAppId,
                        Payload=new byte[]{0},
                    },
                },
                CommandsOnPressedShortAndHold = new List<Command>()
                {
                     new Command()
                    {
                        CommandType=CommandType.DOWN,
                        TargetAppId=targetAppId,
                        Payload=new byte[]{1},
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId=targetAppId,
                    },
                },
            };
            list.Add(up);
            list.Add(down);
        }
    }
}
