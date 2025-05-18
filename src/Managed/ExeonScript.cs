using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class ExeonScript
    {
        public static void Init()
        {
            Console.Debug("C# Scripting working");
        }

        public static void Update()
        {
            Scene sampleScene = SceneManager.GetScene("SampleScene");

            GameObject obj = sampleScene.GetObject("Sponza Atrium");
            Transform transform = obj.transform;
            // transform.Translate(1.0f, 0.0f, 0.0f);
        }
    }
}
