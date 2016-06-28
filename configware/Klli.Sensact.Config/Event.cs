using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Klli.Sensact.Config
{
    public class Event
    {
        public string SourceAppId;
        public EventType EventType; //Prüfung: Kann die SourceApp dieses Event generieren?

        public override int GetHashCode()
        {

            int prime = 31;
            int result = 1;
            result = prime * result + ((SourceAppId == null) ? 0 : SourceAppId.GetHashCode());
            result = prime * result + EventType.GetHashCode();
            return result;
        }
        public override bool Equals(object obj)
        {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (GetType() != obj.GetType())
                return false;
            Event other = (Event)obj;
            if (SourceAppId == null)
            {
                if (other.SourceAppId != null)
                    return false;
            }
            else if (!SourceAppId.Equals(other.SourceAppId))
                return false;
            if (!EventType.Equals(other.EventType))
                return false;
            return true;
        }
    }
}
