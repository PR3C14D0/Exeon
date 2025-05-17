using System;
using System.Runtime.CompilerServices;

namespace Exeon
{
    public class ExeonScript
    {
        public static void Init()
        {
            Scene sampleScene = SceneManager.GetScene("SampleScene");

            Console.Debug(sampleScene.ObjectExists("Sponza Atrium").ToString());
        }
    }
}
