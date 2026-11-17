using System.Globalization;
using System.Text;
using System.Text.RegularExpressions;
namespace Klli.Sensact.Model.Common.Applications
{
    /// <summary>Hoehe der Sonne, ab der "Tag" ist (C++: sunsetsunrise::eDawn).</summary>
    public enum Dawn
    {
        NORMAL,
        CIVIL,
        NAUTIC,
        ASTRONOMIC,
    }

    /// <summary>Rollladen-Timer (C++: sensact::apps::cBlindsTimer): oeffnet die Rolllaeden zur
    /// Morgen- und schliesst sie zur Abenddaemmerung, mit Versatz und Zufallsanteil je Rollladen.</summary>
    public class BlindsTimerApplication : ActorApplication
    {
        // gesendete Kommandos: blindstimer.cc
        public override IEnumerable<(ushort Target, CommandType Command)> SentCommands() => To(BlindTargets, CommandType.SET_VERTICAL_TARGET);

        public BlindsTimerApplication(ushort ApplicationId, string ApplicationName, string ApplicationDescription, ISet<ushort> BlindTargets,
            double LatitudeDeg, double LongitudeDeg, Dawn Dawn, int OpenOffsetMinutes, int CloseOffsetMinutes, ushort MaxRandomMinutes)
            : base(ApplicationId, ApplicationName, ApplicationDescription)
        {
            this.BlindTargets = BlindTargets;
            this.LatitudeDeg = LatitudeDeg;
            this.LongitudeDeg = LongitudeDeg;
            this.Dawn = Dawn;
            this.OpenOffsetMinutes = OpenOffsetMinutes;
            this.CloseOffsetMinutes = CloseOffsetMinutes;
            this.MaxRandomMinutes = MaxRandomMinutes;
        }
        public ISet<ushort> BlindTargets { get; }
        public double LatitudeDeg { get; }
        public double LongitudeDeg { get; }
        public Dawn Dawn { get; }
        public int OpenOffsetMinutes { get; }
        public int CloseOffsetMinutes { get; }
        public ushort MaxRandomMinutes { get; }

        [SensactCommandMethod]
        public override void OnONCommand(uint autoReturnToOffMsecs) { }
        [SensactCommandMethod]
        public override void OnOFFCommand(uint autoReturnToOnMsecs) { }

        public override HashSet<EventType> ICanSendTheseEvents() => new();

        public override string CheckAndAddUsedPins(HashSet<string> usedInputPins, HashSet<string> usedOutputPins) => string.Empty;

        public override Regex AppNameRegex => FLOOR_ROOM_Regex("BLTIM");

        public override string GenerateCPPConstructor(ModelContainerForCodeGenerator m)
        {
            var sb = new StringBuilder();
            sb.AppendFormat(CultureInfo.InvariantCulture,
                "sensact::apps::cBlindsTimer {0}(eApplicationID::{0}, {1}, {2}, {3}, sunsetsunrise::eDawn::{4}, {5}, {6}, {7})",
                ApplicationName, VectorOfApplicationIds(BlindTargets, m), LatitudeDeg, LongitudeDeg, Dawn,
                OpenOffsetMinutes, CloseOffsetMinutes, MaxRandomMinutes);
            return sb.ToString();
        }

        public override string GenerateTypescriptConstructor(ModelContainerForCodeGenerator m)
        {
            var sb = new StringBuilder();
            sb.AppendFormat("new Apps.BlindsTimerApplication(ApplicationId.{0}, '{1}', ctx)", ApplicationName, ApplicationDescription);
            return sb.ToString();
        }
    }
}
