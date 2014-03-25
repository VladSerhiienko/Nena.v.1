using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using Isoclus.Mechanism;

namespace Isoclus.Interaction
{
    using ClusterCollectionImpl = System.Collections.Generic.SortedDictionary<string, Cluster>;
    using PointCollectionImpl = System.Collections.Generic.SortedDictionary<string, Point>;
    using ClusterCollection = System.Collections.Generic.IDictionary<string, Cluster>;
    using PointCollection = System.Collections.Generic.IDictionary<string, Point>;
    using System.Collections.Generic;
    using System.Linq;
    using System;

    class Layout
    {
        static class TargetDenotation
        {
            const string Points = "Points";
            const string Clusters = "Clusters";
            const string Config = "Config";

            public static Target Target(string Line)
            {
                if (Line.Equals(Points)) return Layout.Target.Points;
                else if (Line.Equals(Clusters)) return Layout.Target.Clusters;
                else if (Line.Equals(Config)) return Layout.Target.Config;
                else return Layout.Target.Data;
            }
        }

        enum Target
        {
            Points, 
            Clusters,
            Config,
            Data
        }

        TextReader Stream;

        string Filepath 
        {
            set
            {

                // Check whether the provided value is correct
                if (value != null)
                    if (!System.IO.File.Exists(value)) 
                        throw new FileNotFoundException(
                            "Invalid file path."
                            );
                    else
                    {
                        Stream = File.OpenText(value);
                        //Stream = File.Open(value, FileMode.Open);
                    }
                else
                    throw new ArgumentNullException(
                        "File path required."
                        );
            }
        }

        Layout()
        {
        }

        IEnumerable<string> ByLine()
        {
            while (true)
            {
                var Task = Stream.ReadLine();
                if (Task == null) break;
                yield return Task;
            }
        }

        public static void Startup(string[] Args)
        {
            var Interactor = new Layout();

            try
            {
                // This operation can throw an exception but there is nothing one can do here.
                // All these thrown exceptions should be cared if in a caller function for now. 
                Interactor.Filepath = Args[0];
            }
            catch
            {
                ("ERR: Failed to create file stream.").ToOutputWindow();
                throw;
            }

            Config Settings = new Config();
            PointCollection Points = new PointCollectionImpl();
            ClusterCollection Clusters = new ClusterCollectionImpl();

            char[] SplitSeps = { ' ', ',', '\t', '\r', '\n' };
            var SplitOpts = StringSplitOptions.RemoveEmptyEntries;

            var PreviousTarget = Layout.Target.Clusters;
            var Target = Layout.Target.Data;
            Action<string> ProcessTarget = Line =>
            {
                ("INF: " + Line).ToOutputWindow();
                var Params = Line.Split(SplitSeps, SplitOpts);

                switch (PreviousTarget)
                {
                    case Target.Points:
                        {
                            bool Status = true;
                            float X = .0f, Y = .0f;

                            if (Params.Length < 2) throw new InvalidDataException(
                                "Unsupported point format."
                                );

                            Status = float.TryParse(Params[0].Trim(), out X);
                            if (!Status) throw new InvalidDataException(
                                "Failed to parse Point X (" + Params[0] + " was provided)."
                                );

                            Status = float.TryParse(Params[1].Trim(), out Y);
                            if (!Status) throw new InvalidDataException(
                                "Failed to parse Point Y (" + Params[1] + " was provided.)"
                                );

                            Point Point = null;
                            if (Params.Length >= 3)
                            {
                                Point = new Point(X, Y, Special: true);
                                Point.Id = Params[3];
                            }
                            else Point = new Point(X, Y);

                            Points.Add(Point.Id, Point);
                        } break;
                    case Target.Clusters:
                        {
                            bool Status = true;
                            float X = .0f, Y = .0f;

                            if (Params.Length < 2) throw new InvalidDataException(
                                "Unsupported cluster format."
                                );

                            Point Center = null;
                            Cluster Cluster = new Cluster();
                            if (Params[0].Equals("Random"))
                            {
                                if (Params.Length >= 3)
                                {
                                    Status = float.TryParse(Params[1].Trim(), out X);
                                    if (!Status) throw new InvalidDataException(
                                        "Failed to parse Bound 1 (" + Params[1] + " was provided)."
                                        );

                                    Status = float.TryParse(Params[2].Trim(), out Y);
                                    if (!Status) throw new InvalidDataException(
                                        "Failed to parse Bound 2 (" + Params[2] + " was provided)."
                                        );

                                    Random Rand = new Random();
                                    if (Params.Length >= 4)
                                    {
                                        Center = new Point(
                                            Rand.Next((int)X, (int)Y),
                                            Rand.Next((int)X, (int)Y),
                                            Special: true
                                            );

                                        Center.Id = Params[3];
                                    }
                                    else
                                    {
                                        Center = new Point(
                                            Rand.Next((int)X, (int)Y),
                                            Rand.Next((int)X, (int)Y)
                                            );
                                    }

                                    if (Params.Length >= 5)
                                    {
                                        Cluster.Id = Params[4];
                                    }
                                }
                            }
                            else
                            {
                                Status = float.TryParse(Params[0].Trim(), out X);
                                if (!Status) throw new InvalidDataException(
                                    "Failed to parse Center X (" + Params[0] + " was provided)."
                                    );

                                Status = float.TryParse(Params[1].Trim(), out Y);
                                if (!Status) throw new InvalidDataException(
                                    "Failed to parse Center Y (" + Params[1] + " was provided)."
                                    );

                                if (Params.Length >= 3)
                                {
                                    Center = new Point(X, Y, Special: true);
                                    Center.Id = Params[2];
                                }
                                else
                                {
                                    Center = new Point(X, Y);
                                }

                                if (Params.Length >= 4)
                                {
                                    Cluster.Id = Params[3];
                                }
                            }

                            Cluster.Center = Center;

                            Clusters.Add(Cluster.Id, Cluster);
                        } break;
                    case Target.Config:
                        Settings.Edit(Params);
                        break;
                }

                GC.Collect();
            };

            foreach (var LineTask in Interactor.ByLine())
            {
                if (LineTask != null)
                {
                    string Line = string.Empty;
                    switch (Target = TargetDenotation.Target(Line = (LineTask).Trim()))
                    {
                        case Layout.Target.Data:
                            ProcessTarget(Line);
                            break;
                        default:
                            PreviousTarget = Target;
                            break;
                    }
                }
            }

            Settings.Kinit = Clusters.Count;

            TextWriter Writer = File.CreateText(Args[1]);
            Writer.WriteLine(DateTime.Now + "\n ISOCLUS OUTPUT: \n\n");

            Console.SetOut(Writer);
            Clusterizer Isoclus = new Clusterizer(
                Points, Clusters,
                Settings
                );

            ("\nINF: Initial points:").ToOutputWindow();
            Points.ToOutputWindow();

            ("\nINF: Initial clusters:").ToOutputWindow();
            Clusters.ToOutputWindow();

            Isoclus.Clusterize();

            Writer.Flush();
            Writer.Close();

            Settings = null;
            Clusters = null;
            Isoclus = null;
            Points = null;

            GC.Collect();
        }
    }

}
