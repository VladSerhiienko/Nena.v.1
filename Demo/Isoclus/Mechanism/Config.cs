namespace Isoclus.Mechanism
{
    using System;
    using System.Reflection;

    /// <summary>
    /// Clusterizer configuration
    /// </summary>
    class Config
    {
        /// <summary>
        /// Initial number of clusters.
        /// </summary>
        public int Kinit;
        /// <summary>
        /// Minimum number of points that can form a cluster.
        /// </summary>
        public int Nmin;
        /// <summary>
        /// Max number of iterations.
        /// </summary>
        public int Imax;
        /// <summary>
        /// Max deviation of points from their cluster centers.
        /// </summary>
        public float Dmax;
        /// <summary>
        /// Min required distance between two clusters.
        /// </summary>
        public float Lmin;
        /// <summary>
        /// Max number of cluster pairs that can be merged per iteration.
        /// </summary>
        public int Pmax;

        /// <summary>
        /// Apply default settings. 
        /// </summary>
        public Config()
        {
            Kinit = 2;
            Nmin = 1;
            Imax = 20;
            Dmax = 1.9f;
            Lmin = 2.5f;
            Pmax = 10;
        }

        public void Edit(string[] Args)
        {
            var TypeInfo = GetType();
            var Fields = TypeInfo.GetFields();
            foreach (var Field in Fields)
            {
                if (Field.Name.ToLowerInvariant().Equals(Args[0].ToLowerInvariant()))
                {
                    var Int = typeof(int);
                    var Float = typeof(float);

                    if (Field.FieldType == Float)
                    {
                        float Result = 0.0f;
                        bool Status = float.TryParse(Args[1], out Result);

                        if (!Status)
                        {
                            ("Failed to bind value " + Args[1] + " to field " + Field.Name + ". " + 
                                Float.ToString() + " type expected. The Default value is " + 
                                Field.GetValue(this) + "\n").ToOutputWindow();
                        }
                        else
                        {
                            try
                            {
                                Field.SetValue(this, Result);
                            }
                            catch (System.Exception e)
                            {
                                (e.Message).ToOutputWindow();
                                (e.StackTrace).ToOutputWindow();
                                ("\n").ToOutputWindow();
                            }
                        }
                    }
                    else if (Field.FieldType == Int)
                    {
                        int Result = 0;
                        bool Status = int.TryParse(Args[1], out Result);

                        if (!Status)
                        {
                            ("Failed to bind value " + Args[1] + " to field " + Field.Name + ". " +
                                Int.ToString() + " type expected. The Default value is " +
                                Field.GetValue(this) + "\n").ToOutputWindow();
                        }
                        else
                        {
                            try
                            {
                                Field.SetValue(this, Result);
                            }
                            catch (System.Exception e)
                            {
                                (e.Message).ToOutputWindow();
                                (e.StackTrace).ToOutputWindow();
                                ("\n").ToOutputWindow();
                            }
                        }
                    }
                }
            }


        }

        public override string ToString()
        {
            var TypeInfo = GetType();
            var Fields = TypeInfo.GetFields();
            string Output = "Config[";
            foreach (var Field in Fields)
                Output += "[" + Field.Name + "=" + 
                    Field.GetValue(this) + "]";
            Output += "]";
            return Output;
        }
    }
}


