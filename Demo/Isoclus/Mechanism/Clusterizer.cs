namespace Isoclus.Mechanism
{
    using ClusterCollectionImpl = System.Collections.Generic.SortedDictionary<string, Cluster>;
    using PointCollectionImpl = System.Collections.Generic.SortedDictionary<string, Point>;
    using ClusterCollection = System.Collections.Generic.IDictionary<string, Cluster>;
    using PointCollection = System.Collections.Generic.IDictionary<string, Point>;
    using System.Collections.Generic;
    using System.Linq;
    using System;

    /// <summary>
    /// ISOCLUS algorithm implementation 
    /// </summary>
    class Clusterizer
    {
        class ClusterPair
        {
            public string LeftId { get; set; }
            public string RightId { get; set; }
            public bool Involved { get; set; }
            public float Distance { get; set; }
        }

        class ClusterPairComparer : IEqualityComparer<ClusterPair>
        {
            public bool Equals(ClusterPair LeftPair, ClusterPair RightPair)
            {
                return (LeftPair.LeftId == RightPair.RightId) || (LeftPair.RightId == RightPair.LeftId) ||
                    (LeftPair.LeftId == RightPair.LeftId) || (LeftPair.RightId == RightPair.RightId);

                //return ((LeftPair.LeftId == RightPair.RightId) &&
                //    (LeftPair.RightId == RightPair.LeftId)) ||
                //    ((LeftPair.LeftId == RightPair.LeftId) &&
                //    (LeftPair.RightId == RightPair.RightId));
            }

            public int GetHashCode(ClusterPair Pair)
            {
                return Pair.GetHashCode();
            }
        }

        private Config Settings;
        private PointCollection Points;
        private ClusterCollection Clusters;

        public int K
        {
            get
            {
                return Clusters.Count;
            }
        }

        public Clusterizer(PointCollection Elements, ClusterCollection Containters)
        {
            Points = Elements ?? new PointCollectionImpl();
            Clusters = Containters ?? new ClusterCollectionImpl();
        }

        public Clusterizer(PointCollection Points, ClusterCollection Clusters, Config Config) 
            : this(Points, Clusters)
        {
            if (Config == null)
            {
                throw new ArgumentNullException(
                    "Configuration required."
                    );
            }
            else
            {
                Settings = Config;
            }
        }

        public void Clusterize(Config Config = null)
        {
            // Check whether configuration was provided 

            if (Config != null)
            {
                Settings = Config;
                GC.Collect();
            }
            else if (Settings == null)
            {
                // No settings provided
                throw new NullReferenceException(
                    "Configuration required."
                    );
            }

            if (Clusters == null || Points == null)
            {
                // No settings provided
                throw new NullReferenceException(
                    "Initial data required."
                    );
            }

            //("\nINF: The settings are provided, everything is ok and we can start.").ToOutputWindow();

            int CurrentIteration = 1;
            while (CurrentIteration <= Settings.Imax)
            {
                ("\nINF: Iteration {0}.").ToOutputWindow(CurrentIteration);

                ("\nINF: Step 2.").ToOutputWindow();
                AssociatePointsWithClusters(); // Go to Step 2.

                ("\nINF: Step 3.").ToOutputWindow();
                RemoveSmallClusters(); // Go to Step 3.

                ("\nINF: Step 4.").ToOutputWindow();
                if (AdjustClusterCenters())
                    foreach (var Cluster in Clusters)
                        Cluster.Value.Whos();// Go to Step 4.


                ("\nINF: Step 5.").ToOutputWindow();
                AdjustClusters(CurrentIteration); // Go to Step 5.
                CurrentIteration++; // Inc
            }

            ("INF: Clusterized Data:").ToOutputWindow();
            foreach (var Cluster in Clusters)
                Cluster.Value.Whos();
        }

        /// <summary>
        /// Step 2. Assign each point to its closest cluster center. 
        /// Point collection would be completely cleared. 
        /// </summary>
        public void AssociatePointsWithClusters()
        {
            ("\nINF: Assign each point to its closest cluster center:").ToOutputWindow();

            foreach (var Point in Points)
            {
                // The cluster to associate point with. 
                string ClusterId = string.Empty;
                // To trace the closest cluster. 
                float MinDistance = float.MaxValue;

                // Find the nearest cluster
                foreach (var Cluster in Clusters)
                {
                    float Distance = Cluster.Value.EuclideanLengthTo(Point.Value);
                    ("INF: Euclidean Length From {0} To {1} Is {2}").ToOutputWindow(
                        Point.Value.ToString(), // 0
                        Cluster.Value, // 1
                        Distance // 2
                        );

                    if (MinDistance > Distance)
                    {
                        MinDistance = Distance;
                        ClusterId = Cluster.Key;
                    }
                }

                // Associate point with the nearest cluster
                Clusters[ClusterId].AssociatePoint(Point.Value);
            }

            // Remove all the points as they were associated with the clusters. 
            Points.Clear();
        }

        /// <summary>
        /// Step 3. Remove all the clusters size of which is less then Nmin (Settings)
        /// </summary>
        public bool RemoveSmallClusters()
        {
            // Collection of the cluster keys to remove
            List<string> RemovedClusters = new List<string>();

            foreach (var Cluster in Clusters)
            {
                if (Cluster.Value.N < Settings.Nmin)
                {
                    RemovedClusters.Add(Cluster.Key);
                }
            }

            if (RemovedClusters.Count > 0)
            {
                ("\nINF: Small clusters:").ToOutputWindow();
                RemovedClusters.ToOutputWindow();
            }
            else
            {
                ("\nINF: No small clusters detected.").ToOutputWindow();
                return false;
            }

            RemovedClusters.ForEach(
                ClusterKey => 
                    {
                        // Save all the points
                        foreach (var Point in Clusters[ClusterKey].AssociatedPoints)
                            Points.Add(Point);
                        // Remove cluster
                        Clusters.Remove(ClusterKey);
                    }
                );

            ("\nDisassociated Points:").ToOutputWindow();
            Points.ToOutputWindow();

            return true;
        }


        /// <summary>
        /// Step 4. Adjust cluster centers to their point set centroids. 
        /// </summary>
        public bool AdjustClusterCenters()
        {
            bool Result = false;
            foreach (var Cluster in Clusters)
                Result |= Cluster.Value.AdjustCenter();
            return Result;
        }

        /// <summary>
        /// Step 5. In this algorithm stage the clusters will be merged or/and split. 
        /// Its rather large and complex. 
        /// </summary>
        public void AdjustClusters(int CurrentIteration)
        {
            // Calculate average distances and the overall average distance 

            // Max distance
            float OverallAverageDistance = 0.0f;

            foreach (var Cluster in Clusters)
            {
                OverallAverageDistance += Cluster.Value.AverageDistance;// *Cluster.Value.N;
            }

            OverallAverageDistance /= K;


            // If (its the last iteration)
            if ((CurrentIteration == Settings.Imax))
            {
                Settings.Lmin = 0; // Zero the min required distance
                MergeClusters(); // Go to Step 9, then 10, 11.
                return; // And go to Step 2.
            }
            // or if (2K > Kinit and even iteration or K >= 2Kinit)
            else if (((2 * K) > Settings.Kinit) && ((CurrentIteration % 2) == 0)) // ||
                //(K >= 2 * Settings.Kinit))
            {
                MergeClusters(); // Go to Step 9, then 10, 11.
                return; // And go to Step 2.
            }

            // Go to Step 7, then 8.
            // We did not merge any clusters so lets try to split them 
            if (SplitClusters(OverallAverageDistance) == 0)
                // Merge if there was no splits previously
                MergeClusters(); // Go to Step 9, then 10, 11.
        }

        public int SplitClusters(float OverallAverageDistance)
        {
            ("INF: Try Split Clusters").ToOutputWindow();
            var NumSeparedClusters = 0;
            // Its not allowed to modify sequence while foreach(...) loop
            // We will need the temporary container for the new clusters 
            ICollection<Cluster> SplitClusters = new List<Cluster>();

            // Try to split each cluster 
            foreach (var Left in Clusters)
            {
                var Direction = 0;

                bool Condition1 = (Left.Value.Deviation.MaxCoord(out Direction) > Settings.Dmax);
                bool Condition2 = (Left.Value.AverageDistance >= OverallAverageDistance);
                bool Condition3 = (Left.Value.N > 2 * (Settings.Nmin + 1));
                bool Condition4 = (K <= Settings.Kinit / 2);

                // This condition should be satisfied to perform split operation over the cluster
                if (Condition1 && ((Condition2 && Condition3) || Condition4))
                {
                    foreach (var Point in Left.Value.AssociatedPoints)
                        Points.Add(Point);

                    ("\nDisassociated Points:").ToOutputWindow();
                    Points.ToOutputWindow();

                    Left.Value.AssociatedPoints.Clear();
                    var Right = Left.Value.SplitWithRespectTo(OverallAverageDistance, Direction);
                    SplitClusters.Add(Right);
                    NumSeparedClusters++;
                }
            }

            foreach (var Cluster in SplitClusters)
                Clusters.Add(Cluster.Id, Cluster);

            SplitClusters.Clear();
            GC.Collect();

            return NumSeparedClusters;
        }


        /// <summary>
        /// Steps 9, 10, 11. Merge clusters
        /// </summary>
        public void MergeClusters()
        {
            ("INF: Try Merge Clusters").ToOutputWindow();

            // Compute the pairwise intercluster distances and sort them in the increasing order. 
            // ClusterPairComparer instance ensures we would not have pairs with at least two same clusters. 
            // Consider this: (2, 3), (2, 5), (5, 4). The set (2, 5) will be removed because cluster (2) 
            // is already in the first set (2, 3). 
            List<ClusterPair> ClusterPairs = (from Cluster1 in Clusters from Cluster2 in Clusters
                let Distance = Cluster1.Value.EuclideanLengthTo(Cluster2.Value.Center)
                where (Cluster1.Key != Cluster2.Key) && (Distance <= Settings.Lmin)
                orderby Distance ascending select new ClusterPair {
                    Involved = false, LeftId = Cluster1.Key, RightId = Cluster2.Key,
                    Distance = Distance
                //}).Distinct(new ClusterPairComparer()).ToList();
                }).ToList();

            // Take only the provided pair count 
            if (ClusterPairs.Count > Settings.Pmax)
                ClusterPairs = ClusterPairs
                    .Take(Settings.Pmax)
                    .ToList();

            int MergeIteration = 0;
            while (MergeIteration < ClusterPairs.Count)
            {
                if (!ClusterPairs[MergeIteration].Involved)
                {
                    ClusterPairs[MergeIteration].Involved = true;
                    ClusterPairs.ForEach( Pair => {
                            if (Pair.LeftId == ClusterPairs[MergeIteration].LeftId || 
                                Pair.LeftId == ClusterPairs[MergeIteration].RightId || 
                                Pair.RightId == ClusterPairs[MergeIteration].RightId || 
                                Pair.RightId == ClusterPairs[MergeIteration].LeftId) {
                                Pair.Involved = true;
                            }
                        }
                    );

                    // Basically, its the left cluster with the the 
                    // rights cluster points and updated center
                    Cluster Merged = Clusters[ClusterPairs[MergeIteration].LeftId]
                        .MergeWith(Clusters[ClusterPairs[MergeIteration].RightId]);

                    // Remove right cluster and replace the left cluster with the new merged cluster
                    Clusters.Remove(ClusterPairs[MergeIteration].RightId);
                    Clusters[ClusterPairs[MergeIteration].LeftId] = Merged;
                }

                // Move to the next pair if any. 
                MergeIteration++;
            }
        }
    }
}
