namespace Klli.Sensact.Model.Common.Nodes
{
    public class SensactUp02 : Node
    {
        public const int Pin_ROTD = 16+15;
        public const int Pin_xROTD = 16 + 3;
        public const int IO1 = 16;
        public const int IO2 = 17;
        public const int IO3 = 6;
        public const int IO4 = 7;
        public const int IO5 = 4;
        public const int IO6 = 5;
        public const int IO7 = 2;
        public const int IO8 = 3;
        
        public SensactUp02(ushort nodeId, string nodeName) : base(nodeId, nodeName)
        {

        }
    }
}
