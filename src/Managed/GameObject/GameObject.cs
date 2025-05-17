using System;
using System.Runtime.CompilerServices;
using Exeon;

namespace Exeon
{
    public class GameObject
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr GetTransform_Impl(UIntPtr ptr);

        private Transform GetTransform() {
            UIntPtr ptr = GetTransform_Impl(this.nativePtr);
            return new Transform(ptr);
        }

        private UIntPtr nativePtr;
        public GameObject(UIntPtr ptr)
        {
            this.nativePtr = ptr;
        }

        public Transform transform {
            get => GetTransform();
        }
    }
}