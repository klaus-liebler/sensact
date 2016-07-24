using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Klli.Sensact.TestGui
{
    public class CommandSpecification
    {
        public ushort applicationIdAsUshort;
        public byte CommandIdAsInt;
        //Matching von Controls zu Parametern
        public List<Control2Parameter> C2Ps=new List<Control2Parameter>();

    }

    public class Control2Parameter
    {
        public Control TheControl;
        public ParameterInfo TheParameter;
    }
}
