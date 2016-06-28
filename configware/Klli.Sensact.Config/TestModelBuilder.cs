﻿using Klli.Sensact.Config.Applications;
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
        #region Model for test all Inputs
        public static Model BuildOld()
        {
            var cmdList = new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.TOGGLE,
                                    TargetAppId=_(ID.BLIND_L1_BATH_J1),// "POWIT_XX_XXX_1"
                                }
                            };

            Model model = new Model();
            model.Nodes = new List<Node>(){
                new SensactHs07()
                {
                    Id="EG2",
                    Applications=new List<SensactApplication>()
                    {
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I15_XXX",
                            InputRessource=InputPin.I15,
                            CommandsOnPressed=cmdList
                        },
                    }
                },
                new SensactHs07()
                {
                    Id="EG1",
                    Applications=new List<SensactApplication>()
                    {

                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I15",
                            InputRessource=InputPin.I15,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I13",
                            InputRessource=InputPin.I13,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I12",
                            InputRessource=InputPin.I12,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I11",
                            InputRessource=InputPin.I11,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I10",
                            InputRessource=InputPin.I10,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I09",
                            InputRessource=InputPin.I09,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I08",
                            InputRessource=InputPin.I08,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I07",
                            InputRessource=InputPin.I07,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I06",
                            InputRessource=InputPin.I06,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I03",
                            InputRessource=InputPin.I03,
                            CommandsOnPressed=cmdList
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I02",
                            InputRessource=InputPin.I02,
                            CommandsOnPressed=new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.UP,
                                    TargetAppId="BLIND_XX_XXX"
                                }
                            },
                            CommandsOnReleasedLong = new List<Command>()
                            {
                                 new Command()
                                {
                                    CommandType=CommandType.STOP,
                                    TargetAppId="BLIND_XX_XXX"
                                }
                            }
                        },
                        new PushButtonXApplication()
                        {
                            ApplicationId="PUSHB_XX_XXX_I01",
                            InputRessource=InputPin.I01,
                            CommandsOnPressed=new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.DOWN,
                                    TargetAppId="BLIND_XX_XXX"
                                }
                            },
                            CommandsOnReleasedLong = new List<Command>()
                            {
                                 new Command()
                                {
                                    CommandType=CommandType.STOP,
                                    TargetAppId="BLIND_XX_XXX"
                                }
                            }
                        },
                        new RotaryEncoderApplication()
                        {
                            ApplicationId="ROTAR_XX_XXX_1",
                            InputPushRessource=InputPin.I14,
                            InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                            CommandsOnPressed=new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.TOGGLE,
                                    TargetAppId="PWMTU_XX_XXX"
                                }
                            },
                            CommandsOnTurned=new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.STEP_VERTICAL,
                                    TargetAppId="PWMTU_XX_XXX"
                                }
                            }
                        },
                        new PoweritemApplication()
                        {
                            ApplicationId="POWIT_XX_XXX_1",
                            OutputRessource=OutputPin.O01
                        },
                        new PWMApplication()
                        {
                            ApplicationId="PWM_XX_XXX",
                            OutputRessources=new List<PwmPin>() {PwmPin.P12 },
                            MinimalOnLevel=20,
                            LowMeansLampOn=true
                        },
                        new BlindApplication()
                        {
                            ApplicationId="BLIND_XX_XXX",
                            OpenCloseTimeInSeconds=10,
                            OutputRessourceDown=OutputPin.O11,
                            OutputRessourceUpOrPower=OutputPin.O12
                        }
                    }
                }
            };
            model.Applications = new List<SensactApplication>();
            return model;
        }
        #endregion
        public static Model Build1()
        {

            Model model = new Model();

            Node TEST_HS07 = new Nodes.SensactHs07()
            {
                Id = "TEST_HS07",
                Applications = new List<SensactApplication>()
                    {
                        new PWMApplication
                        {
                            ApplicationId="PWM_XX_XXX_1",
                            LowMeansLampOn=true,
                            MinimalOnLevel=10,
                            InitialStoredTargetLevel=100,
                            StandbyController="STDBY_XX_XXX_1",
                            OutputRessources=new List<PwmPin>() { PwmPin.P01 },
                        },
                        
                        new StandbyController
                        {
                            ApplicationId="STDBY_XX_XXX_1",
                            OutputRessource=OutputPin.O16,
                            WaittimeInMsec=10000,
                        },
                        new PushButtonXApplication
                        {
                            ApplicationId="PUSHB_XX_XXX_PWM1BP",
                            InputRessource=InputPin.I01,
                            CommandsOnReleasedShort = new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.TOGGLE,
                                    TargetAppId="PWM_XX_XXX_1",
                                },
                            },

                            CommandsOnPressedShortAndHold = new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.START,
                                    TargetAppId="PWM_XX_XXX_1",
                                },
                            },

                            CommandsOnReleasedLong = new List<Command>()
                            {
                                new Command()
                                {
                                    CommandType=CommandType.STOP,
                                    TargetAppId="PWM_XX_XXX_1",
                                },
                            },
                        },
                    }
            };
            

            model.Nodes = new List<Node>(){
                TEST_HS07,

            };
            model.Applications = new List<SensactApplication>();
            return model;
        }
        private static SensactApplication BuildInput(int i)
        {
            return new PushButtonXApplication()
            {
                ApplicationId = "PUSHB_XX_XXX_" + i,
                InputRessource = Node.index2input[i],
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

        private static SensactApplication BuildOutput(int i)
        {
            return new PoweritemApplication()
            {
                ApplicationId = "POWIT_XX_XXX_"+i,
                OutputRessource = Node.index2output[i],
            };
        }

        private static SensactApplication BuildPWMOutput(int i)
        {
            return new PWMApplication()
            {
                ApplicationId = "PWM_XX_XXX_" + i,
                OutputRessources = new List<PwmPin>() { Node.index2pwm[i] },
            };
        }

        public static Model BuildDemo()//Demo for Scharmann
        {
            Model model = new Model();

            Node TEST_HS07 = new Nodes.SensactHs07()
            {
                Id = "TEST_HS07",
                Applications = new List<SensactApplication>()
                {
                    new DoorbellApplication
                    {
                        ApplicationId="BELL__DOOR"
                    },
                    new StandbyController
                      {
                          ApplicationId="STDBY_XX_XXX_1",
                          OutputRessource=OutputPin.O01,
                          WaittimeInMsec=10000
                      } ,
                      new PWMApplication
                      {
                          ApplicationId="PWM___XX_XXX_1",
                          InitialStoredTargetLevel=255,
                          LowMeansLampOn=false,
                          MinimalOnLevel=20,
                          OutputRessources=new List<PwmPin>() { PwmPin.P01},
                          StandbyController="STDBY_XX_XXX_1"
                      },
                      new BlindApplication
                      {
                          ApplicationId="BLIND_XX_XXX_1",
                          RelayMode=RelayMode.INTERLOCKED,
                          OpenCloseTimeInSeconds=10,
                          OutputRessourceDown=OutputPin.O02,
                          OutputRessourceUpOrPower=OutputPin.O03,
                      },
                      new PushButtonXApplication
                      {
                          ApplicationId="PUSHB_LX_FRO_1",
                          InputRessource=InputPin.I04,
                          CommandsOnPressed=new List<Command>
                          {
                              new Command
                              {
                                  CommandType=CommandType.START,
                                  TargetAppId="BELL__DOOR",
                              }
                          }
                      }
                }
            };
            TEST_HS07.Applications.AddBlindButtons("_XX_BLN_", InputPin.I01, InputPin.I02, "XX_XXX_1");
            TEST_HS07.Applications.AddOnePushbuttonDimmer("_XX_PUB_1", InputPin.I03, "PWM___XX_XXX_1");



            Node TEST_UP02 = new Nodes.SensactUp02
            {
                Id = "TEST_UP02",
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
                        OutputR=PwmPin.P01,
                        OutputG=PwmPin.P03,
                        OutputB=PwmPin.P05,
                        OutputW=PwmPin.P07,
                    },
                    new RotaryEncoderApplication
                    {
                        ApplicationId="ROTAR_YY_YYY_1",
                        InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                        InputPushRessource=InputPin.ROTAR_PUSH_1,
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
                        OutputRessource=OutputPin.O_LED,
                        AutoOffInterval=3000,
                    },
                    new PushButtonXApplication
                    {
                        ApplicationId="PUSHB_YY_PUB_1",
                        InputRessource=InputPin.I01,
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
            model.Applications = new List<SensactApplication>();
            return model;
        }

        //Test for all Inputs, PoweredOutputs and PWMs of HS07
        public static Model BuildModelToTestAllPins()
        {

            Model model = new Model();

            Node TEST_HS07 = new Nodes.SensactHs07()
            {
                Id = "TEST_HS07",
                Applications = new List<SensactApplication>()
                    {
                        new RotaryEncoderApplication
                        {
                            ApplicationId="ROTAR_XX_XXX_1",
                            InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                            InputPushRessource=InputPin.ROTAR_PUSH_1,
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

            Node TEST_UP02 = new Nodes.SensactUp02
            {
                Id = "TEST_UP02",
                Applications = new List<SensactApplication>
                {
                    new RotaryEncoderApplication
                    {
                        ApplicationId="ROTAR_YY_YYY_1",
                        InputRotaryRessource=RotaryEncoder.ROTARYENCODER_1,
                        InputPushRessource=InputPin.ROTAR_PUSH_1,
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
                        OutputRessource=OutputPin.O_LED,
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
            //TEST_UP02.Applications.AddOnePushbuttonDimmer("YY_YYY_01", InputPin.I01, "PWM___YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_01", InputPin.I01, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_02", InputPin.I02, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_03", InputPin.I03, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_04", InputPin.I04, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_05", InputPin.I05, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_06", InputPin.I06, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_07", InputPin.I07, "POWIT_YY_YYY_01");
            TEST_UP02.Applications.AddToggleButton("YY_YYY_08", InputPin.I08, "POWIT_YY_YYY_01");

            model.Nodes = new List<Node>(){
                TEST_HS07,
                TEST_UP02
                
            };
            for(int i=1;i<=32;i++)
            {
                TEST_HS07.Applications.Add(BuildInput(i));
                TEST_HS07.Applications.Add(BuildOutput(i));
                TEST_HS07.Applications.Add(BuildPWMOutput(i));
            }
            model.Applications = new List<SensactApplication>();
            return model;
        }
    }
}