using System.Reflection;

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
