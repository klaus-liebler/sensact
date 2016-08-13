using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace Klli.Sensact.Config.Applications
{
    public class SensactNodeApplication : SensactApplication
    {
        internal override Regex AppIdRegex
        {
            get
            {
                return new Regex("SNSCT" + REGEX_FLOOR_ROOM_SUFFIX);
            }
        }

        public override string GenerateInitializer(ModelContainer m)
        {

            return string.Empty;
        }

        public override HashSet<EventType> ICanSendTheseEvents()
        {
            return new HashSet<EventType> { EventType.PONG };
        }

        public override HashSet<CommandType> ICanReactOnTheseCommands()
        {
            return new HashSet<CommandType> { CommandType.PING };
        }

        [SensactCommandMethod]
        public override void OnPINGCommand(uint payload)
        {

        }

    }
}
