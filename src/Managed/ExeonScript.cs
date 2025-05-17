using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class Console
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void Debug(string msg);
    }

    public class SceneManager
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UIntPtr GetScene_Impl(string name);
        public static Scene GetScene(string name)
        {
            return new Scene(GetScene_Impl(name));
        }
    }

    public class ExeonScript
    {
        public static void Init()
        {
            Scene sampleScene = SceneManager.GetScene("SampleScene");


        }
    }

    public class Scene
    {
        private UIntPtr nativePtr;

        public Scene(UIntPtr ptr)
        {
            nativePtr = ptr;
        }

        private static extern bool ObjectExists_Impl(UIntPtr ptr, string name);
        public bool ObjectExists(string name)
        {
            return ObjectExists_Impl(this.nativePtr, name);
        }
    }
}
