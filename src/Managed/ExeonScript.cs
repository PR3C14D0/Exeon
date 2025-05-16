using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class ExeonScript
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Debug(string msg);

        public static void Init()
        {
            Debug("ASD");
        }
    }
}
