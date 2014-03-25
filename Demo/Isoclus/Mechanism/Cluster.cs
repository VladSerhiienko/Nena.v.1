namespace Isoclus.Mechanism
{
    using PointCollectionImpl = System.Collections.Generic.SortedDictionary<string, Point>;
    using PointCollection = System.Collections.Generic.IDictionary<string, Point>;
    using System;

    /// <summary>
    /// Left element
    /// </summary>
    class Cluster
    {
        /// <summary>
        ///  Unique name
        /// </summary>
        public string Id;

        /// <summary>
        /// Left center
        /// </summary>
        public Point Center;
        /// <summary>
        /// Left pointers 
        /// </summary>
        public PointCollection AssociatedPoints;

        /// <summary>
        /// Number of associated points
        /// </summary>
        public int N
        {
            get
            {
                return AssociatedPoints.Count;
            }
        }

        /// <summary>
        /// Calculates the average distance of points to their associated cluster center.
        /// </summary>
        public float AverageDistance
        {
            get
            {
                float AverageDistance = 0.0f;
                foreach (var Point in AssociatedPoints)
                    AverageDistance += (Point.Value - Center).Length;
                AverageDistance /= N;
                return AverageDistance;
            }
        }

        /// <summary>
        /// Calculates the standard deviation of each coordinate of points directed from 
        /// the cluster center. 
        /// </summary>
        public Point Deviation
        {
            get
            {
                Point Deviation = new Point(Special: true);
                foreach (var Point in AssociatedPoints)
                    Deviation += (Point.Value - Center).Sqr();
                Deviation /= N;
                Deviation = Deviation.Sqrt();
                return Deviation;
            }
        }

        /// <summary>
        /// Initializes cluster
        /// </summary>
        public Cluster(float XCenter = .0f, float YCenter = .0f, int Index = 0)
        {
            // Initialize data collection
            AssociatedPoints = new PointCollectionImpl();

            // Initialize cluster center
            Center = new Point
            {
                Id = Index == 0 
                    ? "Z" + NextId
                    : "Z" + Index,
                X = XCenter,
                Y = YCenter
            };

            // Assign cluster id
            Id = Index == 0 
                ? "S" + NextId
                : "S" + Index;

            // Correct next cluster index
            if (Index > NextId)
            {
                NextId = Index + 1;
            }

            NextId++;
        }

        /// <summary>
        /// Distance between the cluster center and the provided point
        /// </summary>
        /// <param name="Point">The point to calc the distance to.</param>
        /// <returns>Distance to the provided point.</returns>
        public float EuclideanLengthTo(Point Point)
        {
            return (Point - Center).Length;
        }

        /// <summary>
        /// Calculates the centroid of the associated points
        /// </summary>
        public bool AdjustCenter()
        {
            Point Centroid = new Point(Special:true);
            foreach (var Point in AssociatedPoints)
                Centroid += Point.Value;
            ("\nINF: Center " + Center).ToOutputWindow();
            Centroid /= N;

            bool Result = Center.X == Centroid.X &&
                Center.Y == Centroid.Y;

            Center.X = Centroid.X;
            Center.Y = Centroid.Y;
            ("INF: Updated Center " + Center).ToOutputWindow();

            return Result;
        }

        public bool IsAssociatedPoint(Point Point)
        {
            return AssociatedPoints.ContainsKey(Point.Id);
        }

        public bool IsAssociatedPoint(string PointId)
        {
            return AssociatedPoints.ContainsKey(PointId);
        }

        public void AssociatePoint(Point Point)
        {
            if (!IsAssociatedPoint(Point))
            {
                AssociatedPoints.Add(Point.Id, Point);
                ("INF: Associate {0} With {1}").ToOutputWindow(
                    Point.ToString(), this.ToString()
                    );
            }
            else
            {
                ("ERR: {0} Is Already Associated With {1}").ToOutputWindow(
                    Point.ToString(), this.ToString()
                    );

                throw new ArgumentException(
                    "The point is already associated " + 
                    "with the cluster."
                    );
            }
        }

        public void DisassociatePoint(Point Point)
        {
            if (IsAssociatedPoint(Point))
            {
                AssociatedPoints.Remove(Point.Id);
                ("INF: Disassociate {0} From {1}").ToOutputWindow(
                    Point.ToString(), this.ToString()
                    );
            }
            else
            {
                ("ERR: {0} Is Not Associated With {1}").ToOutputWindow(
                    Point.ToString(), this.ToString()
                    );

                throw new ArgumentException(
                    "The point is not associated " + 
                    "with the cluster."
                    );
            }
        }

        public override string ToString()
        {
            return "Cluster[" + Id + " " + Center + "]";
        }

        public void Whos()
        {
            ToString().ToOutputWindow();
            AssociatedPoints.ToOutputWindowWithArgs("\t{0}");
        }

        static public int NextId
        {
            get; private set;
        }

        static Cluster()
        {
            NextId = 1;
        }
    }

    static class ClusterExtensions
    {
        /// <summary>
        /// Generates the new merged cluster with first cluster id. 
        /// </summary>
        /// <param name="Left">First cluster</param>
        /// <param name="Right">Second cluster</param>
        /// <returns>New merged cluster</returns>
        public static Cluster MergeWith(this Cluster Left, Cluster Right)
        {
            // Create new cluster
            foreach (var Point in Right.AssociatedPoints)
                Left.AssociatePoint(Point.Value);

            // Calculate merged cluster center 
            string Name = Left.Center.Id;
            Left.Center = (Left.Center * Left.N + Right.Center * Right.N) /
                (Left.N + Right.N);
            Left.Center.Id = Name;

            ("\nINF: Merge {2} With {1} {0}").ToOutputWindow(
                Left.Center.ToString(),
                Right.ToString(),
                Left.ToString()
                );

            return Left;
        }

        public static Cluster SplitWithRespectTo(this Cluster Left, float MaxAverageDistance, int Direction)
        {
            Cluster Right = new Cluster();
            string Name = Right.Center.Id;

            Right.Center = (Point)Left.Center.Clone();
            Right.Center.Id = Name;

            switch (Direction)
            {
                case 1:
                    Left.Center.X += MaxAverageDistance;
                    Right.Center.X -= MaxAverageDistance;
                    break;
                case 2:
                    Left.Center.Y += MaxAverageDistance;
                    Right.Center.Y -= MaxAverageDistance;
                    break;
                default:
                    ("ERR: Wrong direction.").ToOutputWindow();
                    throw new IndexOutOfRangeException();
            }

            ("\nINF: Split {0} From {1} (MAD {2} DIR {3})").ToOutputWindow(
                Right.ToString(), Left.ToString(),
                MaxAverageDistance,
                Direction
                );

            return Right;
        }
    }
}
