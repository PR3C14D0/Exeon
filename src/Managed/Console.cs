using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
     public class Console
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Debug(string msg);
    }
}