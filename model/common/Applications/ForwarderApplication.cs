using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Model.Common.Applications
{
    public class ForwarderApplication : ActorApplication
    {
        public ForwarderApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ushort finalTarget):base(ApplicationId, ApplicationName, ApplicationDescription){
            this.FinalTarget=finalTarget;
        }
        public ushort FinalTarget{get;}


        [SensactCommandMethod]
        public override void OnNOPCommand() { }
        [SensactCommandMethod]
        public override void OnRESETCommand() { }
        [SensactCommandMethod]
        public override void OnSTART_IAPCommand() { }
        [SensactCommandMethod]
        public override void OnSTARTCommand() { }
        [SensactCommandMethod]
        public override void OnSTOPCommand() { }
        [SensactCommandMethod]
        public override void OnUPCommand(byte forced) { }
        [SensactCommandMethod]
        public override void OnDOWNCommand(byte forced) { }
        [SensactCommandMethod]
        public override void OnFORWARDCommand() { }
        [SensactCommandMethod]
        public override void OnBACKWARDCommand() { }
        [SensactCommandMethod]
        public override void OnLEFTCommand() { }
        [SensactCommandMethod]
        public override void OnRIGHTCommand() { }
        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs) { }
        [SensactCommandMethod]
        public override void OnON_FILTERCommand(ushort targetApplicationId, uint autoReturnToOffMsecs) { }
        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs) { }
        [SensactCommandMethod]
        public override void OnTOGGLECommand() { }
        [SensactCommandMethod]
        public override void OnTOGGLE_FILTERCommand(ushort targetApplicationId) { }
        [SensactCommandMethod]
        public override void OnTOGGLE_SPECIALCommand() { }
        [SensactCommandMethod]
        public override void OnSET_PARAMCommand() { }
        [SensactCommandMethod]
        public override void OnSET_HORIZONTAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public override void OnSTEP_HORIZONTALCommand(short step) { }
        [SensactCommandMethod]
        public override void OnSET_VERTICAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public override void OnSTEP_VERTICALCommand(short step) { }
        [SensactCommandMethod]
        public override void OnSET_LATERAL_TARGETCommand(ushort target) { }
        [SensactCommandMethod]
        public override void OnSTEP_LATERALCommand(short step) { }
        [SensactCommandMethod]
        public override void OnHEARTBEATCommand(uint sender) { }
        [SensactCommandMethod]
        public override void OnSEND_STATUSCommand() { }
        [SensactCommandMethod]
        public override void OnSET_RGBWCommand(byte R, byte G, byte B, byte W) { }
        /// <summary>
        /// Param is mp3 song id or well known color or light sequence)
        /// </summary>
        /// <param name="signal"></param>
        [SensactCommandMethod]
        public override void OnSET_SIGNALCommand(ushort signal) { }
        /// <summary>
        /// 
        /// </summary>
        /// <param name="payload">just an ubiquitary payload that is used as payload for the PONG</param>
        [SensactCommandMethod]
        public override void OnPINGCommand(uint payload) { }
        /// <summary>
        /// Advises the sensact application to "play" a demo of its capabilities
        /// </summary>
        /// <param name="demostep"></param>
        [SensactCommandMethod]
        public override void OnDEMOCommand(uint demostep) { }

        [SensactCommandMethod]
        public override void OnSET_PWMCommand(uint bitmask, ushort value) { }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType>();
        }

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins)
        {
            return String.Empty;
        }

        public override string GenerateInitializer(ModelContainerForCodeGenerator mc)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendFormat("// Forwarder {0}"+Environment.NewLine, ApplicationName);
            sb.AppendFormat("sensactapps::cForwarder {0}(eApplicationID::{0}, eApplicationID::{1});"+Environment.NewLine+Environment.NewLine, ApplicationName, mc.GetNameFromId(FinalTarget));
            return sb.ToString();
        }
        
        public override string GenerateTypescriptUserInterface(ModelContainerForCodeGenerator m)
        {
            return string.Empty;
        }
        public override Regex AppNameRegex
        {
            get
            {
                return FLOOR_ROOM_Regex("FRWRD");
            }
        }
    }
}
