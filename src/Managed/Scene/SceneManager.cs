using System;
using System.Runtime.CompilerServices;
using Exeon;

namespace Exeon
{
    public class SceneManager
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr GetScene_Impl(string name);
        public static Scene GetScene(string name)
        {
            return new Scene(GetScene_Impl(name));
        }
    }
}