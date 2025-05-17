using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class Scene
    {
        private UIntPtr nativePtr;

        public Scene(UIntPtr ptr)
        {
            nativePtr = ptr;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool ObjectExists_Impl(UIntPtr ptr, string name);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr GetObject_Impl(UIntPtr ptr, string name);

        public bool ObjectExists(string name)
        {
            return ObjectExists_Impl(this.nativePtr, name);
        }

        public GameObject GetObject(string name)
        {
            UIntPtr ptr = GetObject_Impl(this.nativePtr, name);
            return new GameObject(ptr);
        }
    }
}