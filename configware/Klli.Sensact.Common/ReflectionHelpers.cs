using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Klli.Sensact.Common
{
    public static class ReflectionHelpers
    {
        public static bool IsOverride(this MethodInfo m)
        {
            return m.GetBaseDefinition().DeclaringType != m.DeclaringType;
        }

    }
}
