using Klli.Sensact.Config.Applications;
using Klli.Sensact.Config.Nodes;
using System.Collections.Generic;

namespace Klli.Sensact.Config
{
    public static class TestModelBuilder
    {
        private static string _(this ID id)
        {
            return id.ToString();
        }
        
        public static Model BuildRCDemo()
        {

            Model model = new Model("RCDemo");

            Node TEST_HS07 = new Nodes.SensactHs07("SNSAC_L0_TECH_HS07_1")
            {
                Applications = new List<SensactApplication>()
                    {
                        new PWMApplication
                        {
                            ApplicationId="PWM_XX_XXX_1",
                            LowMeansLampOn=true,
                            MinimalOnLevel=10,
                            InitialStoredTargetLevel=255,
                            StandbyController="STDBY_XX_XXX_1",
                            OutputRessources=new List<ushort>() { 1 },
                        },
                        
                        new StandbyControllerApplication
                        {
                            ApplicationId="STDBY_XX_XXX_1",
                            OutputRessource=16,
                            WaittimeInMsec=10000,
                        },
                        new RCEventApplication
                        {
                            ApplicationId="PUSHB_XX_XXX_PWM1BP",
                            EventCode=13694625,
                            CommandsOnPressed= new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.TOGGLE,
                                    TargetAppId="PWM_XX_XXX_1",
                                },
                            },
                        },
                    }
            };
            

            model.Nodes = new List<Node>(){
                TEST_HS07,

            };
            return model;
        }
        private static SensactApplication BuildInput(ushort i)
        {
            return new PushButtonXApplication()
            {
                ApplicationId = "PUSHB_XX_XXX_" + i,
                InputRessource = i,
                CommandsOnReleasedShort = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId="PWM_XX_XXX_"+i,
                    },
                    new Command()
                    {
                        CommandType=CommandType.TOGGLE,
                        TargetAppId="POWIT_XX_XXX_"+(((i-1)%24)+1),
                    },
                },

                CommandsOnPressedShortAndHold = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.START,
                        TargetAppId="PWM_XX_XXX_"+i,
                    },
                },

                CommandsOnReleasedLong = new List<Command>()
                {
                    new Command()
                    {
                        CommandType=CommandType.STOP,
                        TargetAppId="PWM_XX_XXX_"+i,
                    },
                },
                
            };
        }

        private static SensactApplication BuildOutput(ushort i)
        {
            return new PoweritemApplication()
            {
                ApplicationId = "POWIT_XX_XXX_"+i,
                OutputRessource = i,
            };
        }

        private static SensactApplication BuildPWMOutput(ushort i)
        {
            return new PWMApplication()
            {
                ApplicationId = "PWM_XX_XXX_" + i,
                OutputRessources = new List<ushort>() { i },
                LowMeansLampOn = true,
            };
        }

        public static Model BuildLIBARDemo()
        {
            Model model = new Model("LIBARDemo");

            Node TEST_HS07 = new Nodes.SensactHs07("TEST_HS07")
            { 
                Applications = new List<SensactApplication>()
                {
                    new LightbarrierApplication
                    {
                        ApplicationId = "LIBAR_XX_XXX",
                        ActiveSignalLevel = true,
                        BrightnessSensor = _(ID.NO_APPLICATION),
                        FinalTarget = "PWM___XX_XXX",
                        InputRessource = 1,
                    },
                    new PWMApplication
                    {
                        ApplicationId="PWM___XX_XXX",
                        InitialStoredTargetLevel=255,
                        LowMeansLampOn=true,
                        MinimalOnLevel=20,
                        OutputRessources=new List<ushort>() { 1 },
                        StandbyController=_(ID.NO_APPLICATION),
                        AutoOffIntervalMsecs=5000,
                    }
                }
            };
            model.Nodes.Add(TEST_HS07);
            return model;
        }


        public static Model BuildBLINDDemo()
        {
            Model model = new Model("BLINDDemo");
            Node TEST_HS07 = new Nodes.SensactHs07("TEST_HS07");
            model.Nodes.Add(TEST_HS07);
            TEST_HS07.Applications.AddBlindButtons("PUSHB_XX_XXX_UP", "PUSHB_XX_XXX_DO", 1, 2, "BLIND_XX_XXX_1");
            TEST_HS07.Applications.Add(new BlindApplication { ApplicationId = "BLIND_XX_XXX_1", OpenCloseTimeInSeconds = 10, OutputRessourceUpOrPower = 1, OutputRessourceDownOrDirection = 2, RelayMode = RelayMode.INTERLOCKED_ACTIVE_DOWN });
            return model;
        }

        public static Model BuildRGBWDemo()
        {
            Model model = new Model("RGBWDemo");

            Node TEST_HS07 = new Nodes.SensactHs07("TEST_HS07")
            {
                Applications = new List<SensactApplication>()
                {
                    
                      new PushButtonXApplication
                      {
                          ApplicationId="PUSHB_XX_XXX_1",
                          InputRessource=1,
                          CommandsOnPressed=new List<Command>
                          {
                              new Command
                              {
                                  CommandType=CommandType.TOGGLE,
                                  TargetAppId="RGBW__XX_XXX_1",
                              }
                          }
                      },
                      new PushButtonXApplication
                      {
                          ApplicationId="PUSHB_XX_XXX_2",
                          InputRessource=2,
                          CommandsOnPressed=new List<Command>
                          {
                              new Command
                              {
                                  CommandType=CommandType.STEP_VERTICAL,
                                  TargetAppId="RGBW__XX_XXX_1",
                              }
                          }
                      },
                    new RgbwApplication
                    {
                        ApplicationId="RGBW__XX_XXX_1",
                        OutputRessourceR=1,
                        OutputRessourceG=2,
                        OutputRessourceB=3,
                        OutputRessourceW=4,
                        StandbyController=_(ID.NO_APPLICATION),
                        LowMeansLampOn=false,
                    },
                }
            };
            



            Node TEST_UP02 = new Nodes.SensactUp02("TEST_UP02")
            {
                Applications = new List<SensactApplication>
                {
                    new RotaryEncoderApplication
                    {
                        ApplicationId="ROTAR_YY_YYY_1",
                        InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                        InputPushRessource=0,
                        CommandsOnPressed=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="RGBW__YY_YYY_1",
                                CommandType=CommandType.TOGGLE,
                            }
                        },
                        CommandsOnTurned=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="RGBW__YY_YYY_1",
                                CommandType=CommandType.STEP_VERTICAL,
                            }
                        }
                    },
                   new RgbwApplication
                    {
                        ApplicationId="RGBW__YY_YYY_1",
                        OutputRessourceR=1,
                        OutputRessourceG=3,
                        OutputRessourceB=5,
                        OutputRessourceW=7,
                        StandbyController=_(ID.NO_APPLICATION),
                        LowMeansLampOn=true,
                    },

                },

            };
            model.Nodes = new List<Node>(){
                TEST_HS07,
                TEST_UP02

            };
            return model;
        }

        public static Model BuildCANCommunicationDemo()
        {
            Model model = new Model("CANCommunicationDemo");

            Node TEST_HS07 = new Nodes.SensactHs07("TEST_HS07")
            {
                Applications = new List<SensactApplication>()
                {
                    new SoundApplication
                    {
                        ApplicationId="BELL__DOOR"
                    },
                    new StandbyControllerApplication
                      {
                          ApplicationId="STDBY_XX_XXX_1",
                          OutputRessource=1,
                          WaittimeInMsec=10000
                      } ,
                      new PWMApplication
                      {
                          ApplicationId="PWM___XX_XXX_1",
                          InitialStoredTargetLevel=255,
                          LowMeansLampOn=false,
                          MinimalOnLevel=20,
                          OutputRessources=new List<ushort>() {2},
                          StandbyController="STDBY_XX_XXX_1"
                      },
                      new BlindApplication
                      {
                          ApplicationId="BLIND_XX_XXX_1",
                          RelayMode=RelayMode.INTERLOCKED_ACTIVE_DOWN,
                          OpenCloseTimeInSeconds=10,
                          OutputRessourceDownOrDirection=3,
                          OutputRessourceUpOrPower=4,
                      },
                      new PushButtonXApplication
                      {
                          ApplicationId="PUSHB_LX_FRO_1",
                          InputRessource=4,
                          CommandsOnPressed=new List<Command>
                          {
                              new Command
                              {
                                  CommandType=CommandType.SET_SIGNAL,
                                  TargetAppId="BELL__DOOR",
                              }
                          }
                      }
                }
            };
            //TEST_HS07.Applications.AddBlindButtons("_XX_BLN_", InputPin.I01, InputPin.I02, "XX_XXX_1");
            //TEST_HS07.Applications.AddOnePushbuttonDimmer("_XX_PUB_1", InputPin.I03, "PWM___XX_XXX_1");



            Node TEST_UP02 = new Nodes.SensactUp02("TEST_UP02")
            {
                Applications = new List<SensactApplication>
                {
                    /*
                    new BrightnessSensorApplication
                    {
                        ApplicationId="BSENS_ROOF_BH1750",
                        LimitForPassingToggle=300,
                        ToggleTarget="POWIT_YY_YYY_01",
                        SensorName="bh1750",
                    },
                    */
                    new RgbwApplication
                    {
                        ApplicationId="RGBW__YY_YYY_1",
                        OutputRessourceR=1,
                        OutputRessourceG=3,
                        OutputRessourceB=5,
                        OutputRessourceW=7,
                    },
                    new RotaryEncoderApplication
                    {
                        ApplicationId="ROTAR_YY_YYY_1",
                        InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                        InputPushRessource=0,
                        CommandsOnPressed=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="PWM___XX_XXX_1",
                                CommandType=CommandType.TOGGLE,
                            }
                        },
                        CommandsOnTurned=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="PWM___XX_XXX_1",
                                CommandType=CommandType.STEP_VERTICAL,
                            }
                        }
                    },
                    new PoweritemApplication
                    {
                        ApplicationId="POWIT_YY_YYY_01",
                        OutputRessource=0,//TODO
                        AutoOffIntervalMsecs=3000,
                    },
                    new PushButtonXApplication
                    {
                        ApplicationId="PUSHB_YY_PUB_1",
                        InputRessource=1,
                        CommandsOnPressed=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="POWIT_YY_YYY_01",
                                CommandType=CommandType.ON,
                            }
                        }
                    }
                    
                },

            };
            model.Nodes = new List<Node>(){
                TEST_HS07,
                TEST_UP02

            };
            return model;
        }

        //Test for all Inputs, PoweredOutputs and PWMs of HS07
        public static Model BuildModelToTestAllPins()
        {

            Model model = new Model("TestAllPins");

            Node TEST_HS07 = new Nodes.SensactHs07("TEST_HS07")
            {
                Applications = new List<SensactApplication>()
                    {
                        new RotaryEncoderApplication
                        {
                            ApplicationId="ROTAR_XX_XXX_1",
                            InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                            InputPushRessource=0,
                            CommandsOnPressed=new List<Command>
                            {
                                new Command
                                {
                                    TargetAppId="PWM_XX_XXX_1",
                                    CommandType=CommandType.TOGGLE,
                                }
                            },
                            CommandsOnTurned=new List<Command>
                            {
                                new Command
                                {
                                    TargetAppId="PWM_XX_XXX_1",
                                    CommandType=CommandType.STEP_VERTICAL,
                                }
                            }
                        },
                    }
            };

            Node TEST_UP02 = new Nodes.SensactUp02("TEST_UP02")
            {
                Applications = new List<SensactApplication>
                {
                    new RotaryEncoderApplication
                    {
                        ApplicationId="ROTAR_YY_YYY_1",
                        InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                        InputPushRessource=0,
                        CommandsOnPressed=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="PWM_XX_XXX_1",
                                CommandType=CommandType.TOGGLE,
                            }
                        },
                        CommandsOnTurned=new List<Command>
                        {
                            new Command
                            {
                                TargetAppId="PWM_XX_XXX_1",
                                CommandType=CommandType.STEP_VERTICAL,
                            }
                        }
                    },
                    new PoweritemApplication
                    {
                        ApplicationId="POWIT_YY_YYY_01",
                        OutputRessource=0,//TODO
                    },
                    /*
                    new PWMApplication
                    {
                        ApplicationId="PWM___YY_YYY_01",
                        LowMeansLampOn=true,
                        MinimalOnLevel=20,
                        StandbyController=null,
                        OutputRessources=new List<PwmPin>
                        {
                            PwmPin.P16,
                            PwmPin.P17,
                            PwmPin.P18,
                            PwmPin.P19,
                        }
                    }
                    */
                },
                
            };

            model.Nodes = new List<Node>(){
                TEST_HS07,
                TEST_UP02
                
            };
            for(ushort i=1;i<=32;i++)
            {
                TEST_HS07.Applications.Add(BuildInput(i));
                TEST_HS07.Applications.Add(BuildOutput(i));
                TEST_HS07.Applications.Add(BuildPWMOutput(i));
            }
            return model;
        }
    }
}
