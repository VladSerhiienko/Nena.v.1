

namespace Isoclus
{
    using System.Collections.Generic;
    using System.Diagnostics;
    using System;

    static class Air
    {
        public static void ToOutputWindow<TObject>(this IEnumerable<TObject> Objs)
        {
            foreach (var Item in Objs)
                Item.ToString().ToOutputWindow();
        }

        public static void ToOutputWindowWithArgs<TObject>(this IEnumerable<TObject> Objs, string Format, params object[] Args)
        {
            foreach (var Item in Objs) 
                Item.ToString().ToOutputWindow(Args);
        }

        public static void ToOutputWindow(this string Format, params object[] Args)
        {
            string Output = string.Format(Format, Args);
            Console.WriteLine(Output);
            Debug.WriteLine(Output);
        }

        public static void ToConsoleWindow(this string Format, params object[] Args)
        {
            Console.WriteLine(string.Format(Format, Args));
        }
    }
}
