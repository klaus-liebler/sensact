using Klli.Sensact.Model.Common.Nodes;

namespace Klli.Sensact.Model.Common
{
    public class SensactApplicationContainer
    {
        
        public SensactApplicationContainer(Node Node, SensactApplication Application)
        {
            this.Node = Node;
            this.Application = Application;
        }
        public Node Node { get; }
        public SensactApplication Application { get; }

    }
}