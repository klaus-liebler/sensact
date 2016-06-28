using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class AB
    {

        public static void AddToggleButton(this List<SensactApplication> list, ID appId, InputPin inputRessource, ID targetAppId)
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
       
        public static void AddRotaryEncoder(this List<SensactApplication> list, ID appId, RotaryEncoder rotEnc, InputPin inputRessource, ID targetAppId)
        {
            list.Add(BuildRotaryEncoder(appId, rotEnc, inputRessource, targetAppId));
        }
        public static RotaryEncoderApplication BuildRotaryEncoder(ID appId, RotaryEncoder rotEnc, InputPin inputRessource, ID targetAppId)
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
        public static void AddOnePushbuttonDimmer(this List<SensactApplication> list, ID appId, InputPin inputRessource, ID targetAppId)
        {
            list.Add(BuildOnePushbuttonDimmer(appId, inputRessource, targetAppId));
        }
        

        public static PushButtonXApplication BuildOnePushbuttonDimmer(ID appId, InputPin inputRessource, ID targetAppId)
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

            };
        }

        public static void AddTwoDimButtons(this List<SensactApplication> list, ID appIdUp, ID appIdDown, InputPin inputPinUp, InputPin inputPinDown, ID targetAppId)
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

        public static void AddBlindButtons(this List<SensactApplication> list, ID appIdUp, ID appIdDown, InputPin inputPinUp, InputPin inputPinDown, ID targetAppId)
        {
            PushButtonXApplication up = new PushButtonXApplication
            {
                ApplicationId = appIdUp.ToString(),
                InputRessource = inputPinUp,
                CommandsOnPressed = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.UP,
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
            };
            PushButtonXApplication down = new PushButtonXApplication
            {
                ApplicationId = appIdDown.ToString(),
                InputRessource = inputPinDown,
                CommandsOnPressed = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.DOWN,
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
            };
            list.Add(up);
            list.Add(down);
        }
    }
}
