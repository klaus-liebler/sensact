using System.Reflection;
using System.Text;

namespace Klli.Sensact.Model.Common
{
    public static class ReflectionExtensions
    {
        public static bool IsOverride(this MethodInfo m)
        {
            return m.GetBaseDefinition().DeclaringType != m.DeclaringType;
        }

    }

    public static class StringBuilderExtensions
    {
        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object obj)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, obj);
        }

        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object o1, object o2)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, o1, o2);
        }

        public static StringBuilder AFL(this StringBuilder stringBuilder, string formatString, object o1, object o2, object o3)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine, o1, o2, o3);
        }

        public static StringBuilder AF2L(this StringBuilder stringBuilder, string formatString, object o1)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine + Environment.NewLine, o1);
        }

        public static StringBuilder AF2L(this StringBuilder stringBuilder, string formatString, object o1, object o2)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine + Environment.NewLine, o1, o2);
        }

        public static StringBuilder AF2L(this StringBuilder stringBuilder, string formatString, object o1, object o2, object o3)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine + Environment.NewLine, o1, o2, o3);
        }

        public static StringBuilder AF2L(this StringBuilder stringBuilder, string formatString, object o1, object o2, object o3, object o4)
        {
            return stringBuilder.AppendFormat(formatString + Environment.NewLine + Environment.NewLine, o1, o2, o3, o4);
        }

    }
}
