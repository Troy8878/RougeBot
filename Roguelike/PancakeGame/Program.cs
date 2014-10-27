using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace PancakeGame
{
    class Program
    {
        static void Main(string[] args)
        {
            var location = Assembly.GetExecutingAssembly().Location;
            var locpath = new FileInfo(location);
            var locdir = locpath.Directory.FullName;

            var assembly = Assembly.LoadFile(locdir + "\\\\GamePropertyInterop.dll");

            Console.Write("");

            //GamePropertyInterop.GameBase.RunGame();
        }
    }
}
