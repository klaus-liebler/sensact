using Klli.Sensact.Model.Common.Applications;
using Klli.Sensact.Model.Common.Nodes;
using Klli.Sensact.Model.Common;

namespace Klli.Sensact.Common
{
    public abstract class ModelBuilder<ApplicationIdType> where ApplicationIdType:struct,Enum
    {
        protected Model.Common.Model model;
        const ushort DEFAULT_MIN_DIM_LEVEL=512;
        public ModelBuilder(string name){
            model=new Model.Common.Model(name);

        }

        protected abstract void BuildModel();

        public ModelContainer BuildAndFinalizeAndReturnModelContainer(){
            BuildModel();
            return new ModelContainer(model);
        }
        public void AddPlaceholder(Node node, ApplicationIdType appId, string ApplicationDescription)
        {
            node.Applications.Add(new PlaceholderApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription));
        }

        public void AddBlindApplication(Node node, ApplicationIdType appId, string ApplicationDescription, ushort Relay1, ushort Relay2, RelayInterlockMode relMode, uint time_up_secs, uint time_down_secs)
        {
            node.Applications.Add(new BlindApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription, Relay1, Relay2, relMode, time_up_secs, time_down_secs));
        }


        public void AddPWMApplication(Node node, ApplicationIdType appId, string ApplicationDescription, ApplicationIdType StandbyController, ISet<ushort> outputResources, uint AutoOffIntervalMsecs = 0, ushort minimalOnLevel=DEFAULT_MIN_DIM_LEVEL, ushort initialStoredTargetLevel=ushort.MaxValue)
        {
            node.Applications.Add(new SinglePWMApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription, outputResources, minimalOnLevel, initialStoredTargetLevel, AutoOffIntervalMsecs, Convert.ToUInt16(StandbyController)));
        }


        public void AddToggleButton(Node node, ApplicationIdType appId, ushort inputRessource, ISet<ApplicationIdType> targetAppIds)
        {
            node.Applications.Add(new PushButtonSingle2ToggleApplication(Convert.ToUInt16(appId), appId.ToString(), inputRessource, targetAppIds.Select(x=>Convert.ToUInt16(x)).ToHashSet()));
        }

        public void AddToggleButton(Node node, ApplicationIdType appId, ushort inputRessource, ApplicationIdType targetAppId)
        {
            node.Applications.Add(new PushButtonSingle2ToggleApplication(Convert.ToUInt16(appId), appId.ToString(), inputRessource, new HashSet<ushort>{Convert.ToUInt16(targetAppId)}));
        }

        public void AddPowIt(Node node, ApplicationIdType appId, string ApplicationDescription,  ushort outputRessource, uint autoOffIntervalMsecs = 0, PowerState initialPowerState=PowerState.INACTIVE)
        {
            node.Applications.Add(new OnOffApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription, outputRessource, initialPowerState, autoOffIntervalMsecs));
        }

        public void AddSound(Node node, ApplicationIdType appId, string ApplicationDescription, ushort initialVolume=65535, ushort defaultSignalOnToggle=1){
            node.Applications.Add(new SoundApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription, initialVolume, defaultSignalOnToggle));

        }

        public void AddFingerprint(Node node, ApplicationIdType appId, string ApplicationDescription, ApplicationIdType targetAppId1, ApplicationIdType targetAppId2, ApplicationIdType targetAppId3){
            node.Applications.Add(new FingerprintApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription, Convert.ToUInt16(targetAppId1), Convert.ToUInt16(targetAppId2), Convert.ToUInt16(targetAppId3)));

        }
        public void AddMilightControllerApplication(Node node, ApplicationIdType appId, string ApplicationDescription){
            node.Applications.Add(new MilightControllerApplication(Convert.ToUInt16(appId), appId.ToString(), ApplicationDescription));
        }


        public void AddRotaryEncoder(Node node, ApplicationIdType appId, RotaryEncoder rotEnc, ApplicationIdType targetAppId)
        {
            node.Applications.Add(BuildRotaryEncoder(appId, rotEnc, targetAppId));
        }
        public RotaryEncoder2PWMApplication BuildRotaryEncoder(ApplicationIdType appId, RotaryEncoder rotEnc, ApplicationIdType targetAppId)
        {
            return new RotaryEncoder2PWMApplication(Convert.ToUInt16(appId), appId.ToString(), (ushort)rotEnc, Convert.ToUInt16(targetAppId));
        }

        public RotaryEncoder2PWMApplication BuildRotaryEncoder(ushort appId, RotaryEncoder rotEnc, ushort targetAppId)
        {
            return new RotaryEncoder2PWMApplication(Convert.ToUInt16(appId), appId.ToString(), (ushort)rotEnc, (ushort)targetAppId);
        }
        
        public void AddOneDimButton(Node node, ApplicationIdType appId, ushort inputRessource, ApplicationIdType targetAppId)
        {
            node.Applications.Add(BuildOnePushbuttonDimmer(appId, inputRessource, targetAppId));
        }
        
        public PushButtonSingleApplication BuildOnePushbuttonDimmer(ApplicationIdType appId, ushort inputRessource, ApplicationIdType targetAppId)
        {
            return new PushButtonSingle2PwmSingleApplication(Convert.ToUInt16(appId), appId.ToString(), inputRessource, Convert.ToUInt16(targetAppId));
        }

        public void AddTwoDimButtons(Node node, ApplicationIdType appId, ushort inputPinUp, ushort inputPinDown, ApplicationIdType targetAppId)
        {
            node.Applications.Add(new PushButtonDual2PWMApplication(Convert.ToUInt16(appId), appId.ToString(), inputPinUp, inputPinDown, Convert.ToUInt16(targetAppId)));
        }

        public void AddBlindButtons(Node node, ApplicationIdType appId, ushort inputPinUp, ushort inputPinDown, ApplicationIdType targetAppId)
        {
            node.Applications.Add(new PushButtonDual2BlindApplication(Convert.ToUInt16(appId), appId.ToString(), inputPinUp, inputPinDown, Convert.ToUInt16(targetAppId)));
        }
    }
}
