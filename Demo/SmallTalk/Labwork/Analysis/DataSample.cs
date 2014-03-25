using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis
{
    public class DataSample : ICloneable, IEnumerable<DataSample.Observation>
    {
        public class Observation : ICloneable
        {
            protected double[] Params;

            public double Var { get { return Params[0]; } set { Params[0] = value; } }
            public double this[int i] { get { return Params[i + 1]; } set { Params[i + 1] = value; } }

            public Observation(double[] p)
            {
                Params = (double[])p.Clone();
            }

            public object Clone()
            {
                var clone = new Observation(this.Params);
                return clone;
            }
        }

        public List<Observation> Items { get; protected set; }

        public int ParametersCount { get; set; }

        public DataSample()
        {
            Items = new List<Observation>();
        }

        public object Clone()
        {
            var clone = new DataSample();
            clone.ParametersCount = this.ParametersCount;
            clone.Items = new List<Observation>(this.Items);
            return clone;
        }

        public IEnumerator<DataSample.Observation> GetEnumerator()
        {
            return Items.GetEnumerator();
        }

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return Items.GetEnumerator();
        }

        public Observation this[int i]
        {
            get { return Items[(int)i]; }
        }
    }
}
