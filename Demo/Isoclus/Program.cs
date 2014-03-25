using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Isoclus.Mechanism;

namespace Isoclus
{
    using Mechanism;

    using PointCollection = IDictionary<string, Point>;
    using ClusterCollection = IDictionary<string, Cluster>;
    using PointCollectionImpl = SortedDictionary<string, Point>;
    using ClusterCollectionImpl = SortedDictionary<string, Cluster>;
    using Isoclus.Interaction;

    class App
    {
        static void Main(string[] args)
        {
            args.ToOutputWindow();

            Random rand = new Random();
            for (int i = 0; i < 10; i++)
            {
                var value = rand.NextDouble();
                var valueMod1 = value % 1;
                var valueMod2 = value % 2;
                var valueMod100 = value % 100;

                Console.WriteLine(value);
                Console.WriteLine(valueMod1);
                Console.WriteLine(valueMod2);
                Console.WriteLine(valueMod100);
                Console.WriteLine();
            }


            try
            {
                // Args are our input application commands
                Layout.Startup(args);
            }
            catch (System.Exception ex)
            {
                Console.WriteLine(ex.Message);
                Console.WriteLine(ex.StackTrace);
            }
        }

        static void Demo()
        {
            Random Rand = new Random();

            Cluster[] InitialClusters = new Cluster[]
            {
                new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12)),
                //new Cluster(Rand.Next(-1, 12), Rand.Next(-1, 12))
            };

            ("\nINF: Initial clusters:").ToOutputWindow();
            InitialClusters.ToOutputWindow();

            Point[] InitialPoints = new Point[]
            {
                new Point(0, 3, 1),
                new Point(0, 4),
                new Point(0, 6),
                new Point(1, 4),
                new Point(5, 0),
                new Point(6, 2),
                new Point(7, 1),
                new Point(6, 1),
                new Point(5, 5),
                new Point(6, 6),
                new Point(7, 7),
                new Point(8, 9),
            };

            ("\nINF: Initial points:").ToOutputWindow();
            InitialPoints.ToOutputWindow();

            Config Settings = new Config();
            Settings.Kinit = InitialClusters.Length;
            ("\nINF: Initial settings:").ToOutputWindow();
            Settings.ToString().ToOutputWindow();

            Clusterizer Isoclus = new Clusterizer(
                InitialPoints.ToDictionary(Point => Point.Id),
                InitialClusters.ToDictionary(Cluster => Cluster.Id), 
                Settings
                );

            Isoclus.Clusterize();
        }
    }
}
