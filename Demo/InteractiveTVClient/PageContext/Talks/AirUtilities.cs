using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace InteractiveTV
{
    public static class AirUtilities
    {
        public static readonly string Itv = "itv> ";

        public static void ToOutputWindow<TObject>(this IEnumerable<TObject> Objs)
        {
            foreach (var Item in Objs)
                (Itv + Item.ToString()).ToOutputWindow();
        }
        public static void ToOutputWindow(this string Format, params object[] Args)
        {
            string Output = Itv + string.Format(Format, Args);
            Console.WriteLine(Output);
            Debug.WriteLine(Output);
        }
        public static void ToConsoleWindow<TObject>(this IEnumerable<TObject> Objs)
        {
            foreach (var Item in Objs)
                (Itv + Item.ToString()).ToConsoleWindow();
        }
        public static void ToConsoleWindow(this string Format, params object[] Args)
        {
            Console.WriteLine(Itv + string.Format(Format, Args));
        }

        public static void Print<TObject>(this IEnumerable<TObject> Objs)
        {
#if DEBUG
            Objs.ToOutputWindow();
#else
            Objs.ToConsoleWindow();
#endif
        }

        //public static void ToConsoleWindow<TObject>(this IEnumerable<TObject> Objs, string Format, params object[] Args)
        //{
        //    foreach (var Item in Objs)
        //        (Itv + Item.ToString()).ToConsoleWindow(Args);
        //}

        public static void Print(this string Format, params object[] Args)
        {
#if DEBUG
            Format.ToOutputWindow(Args);
#else
            Format.ToConsoleWindow(Args);
#endif
        }

    }
}
