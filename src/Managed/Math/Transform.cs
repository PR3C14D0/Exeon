using System;
using System.Linq.Expressions;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using Exeon;

[StructLayout(LayoutKind.Sequential)]
struct CVector3
{
    public float x;
    public float y;
    public float z;
}


namespace Exeon
{
    public class Transform
    {

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern CVector3 GetLocation_Impl(UIntPtr ptr);

        private Vector3 GetLocation(UIntPtr ptr)
        {
            Vector3 v = new Vector3();
            CVector3 cv = GetLocation_Impl(ptr);
            v.x = cv.x;
            v.y = cv.y;
            v.z = cv.z;

            return v;
        }

        private UIntPtr nativePtr;
        public Transform(UIntPtr ptr)
        {
            this.nativePtr = ptr;
        }

        public Vector3 location
        {
            get => GetLocation(this.nativePtr);
        }
    }
}