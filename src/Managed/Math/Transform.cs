using System;
using System.Runtime.CompilerServices;
using Exeon;

namespace Exeon
{
    public class Transform
    {

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Vector3 GetLocation_Impl(UIntPtr ptr);

        private UIntPtr nativePtr;
        public Transform(UIntPtr ptr)
        {
            this.nativePtr = ptr;
        }

        public Vector3 Location
        {
            get => GetLocation_Impl(this.nativePtr);
        }
    }
}