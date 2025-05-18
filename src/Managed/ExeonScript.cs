using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class ExeonScript
    {
        public static void Init()
        {
            Scene sampleScene = SceneManager.GetScene("SampleScene");

            Console.Debug("C# Scripting working");
            GameObject obj = sampleScene.GetObject("Sponza Atrium");
            Transform transform = obj.transform;
            Console.Debug(transform.location.x.ToString());
        }
    }
}
