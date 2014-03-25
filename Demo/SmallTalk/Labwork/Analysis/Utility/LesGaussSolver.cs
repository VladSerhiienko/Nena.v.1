using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis.Utility
{
    class LesGaussSolver
    {
        public class SolutionNotFoundException : Exception
        { 
            public SolutionNotFoundException(string errorMessage) : 
                base("Solution cannot be found: \r\n" + errorMessage) 
            { } 
        }

        private double[,] m_mVolatileA;
        private double[,] m_mInitialA;
        private double[] m_vVolatileB;
        private double[] m_vInitialB;
        private double[] m_vX;
        private double[] m_vU;
        private double m_dEps;
        private int m_iSize;

        public double[] XVector { get { return m_vX; } }
        public double[] UVector { get { return m_vU; } }

        
        public LesGaussSolver(double[,] a_matrix, double[] b_vector)
            : this(a_matrix, b_vector, 0.0001)
        {
        }

        public LesGaussSolver(double[,] a_matrix, double[] b_vector, double eps)
        {
            if (a_matrix == null || b_vector == null)
                throw new SolutionNotFoundException(
                    "Parameter = null."
                    );

            int b_length = b_vector.Length;
            int a_length = a_matrix.Length;
            if (a_length != b_length * b_length)
                throw new SolutionNotFoundException(
                    "Lines or columns number != b_vector.Length."
                    );


            this.m_mInitialA = a_matrix;
            this.m_mVolatileA = (double[,])a_matrix.Clone();
            this.m_vInitialB = b_vector;
            this.m_vVolatileB = (double[])b_vector.Clone();
            this.m_vX = new double[b_length];
            this.m_vU = new double[b_length];
            this.m_iSize = b_length;
            this.m_dEps = eps;
        }

        public void Solve()
        {
            int[] index = InitializeIndices();
            ForwardStep(index);
            BackwardStep(index);
            EstimateError();
        }

        private int[] InitializeIndices()
        {
            int[] index = new int[m_iSize];
            for (int i = 0; i < index.Length; ++i)
                index[i] = i;
            return index;
        }

        private double MaxInRow(int row, int[] index)
        {
            int max_index = row;
            double max = m_mVolatileA[row, index[max_index]];
            double max_abs = Math.Abs(max);

            for (int cur_index = row + 1; cur_index < m_iSize; ++cur_index)
            {
                double cur = m_mVolatileA[row, index[cur_index]];
                double cur_abs = Math.Abs(cur);
                if (cur_abs > max_abs)
                {
                    max_index = cur_index;
                    max = cur;
                    max_abs = cur_abs;
                }
            }

            if (max_abs < m_dEps)
            {
                if (Math.Abs(m_vVolatileB[row]) > m_dEps) throw new SolutionNotFoundException(
                    "Incompatible linear equation set."
                    );
                else throw new SolutionNotFoundException(
                    "Multitude of solutions."
                    );
            }

            int temp = index[row];
            index[row] = index[max_index];
            index[max_index] = temp;

            return max;
        }

        private void ForwardStep(int[] index)
        {
            for (int i = 0; i < m_iSize; ++i)
            {
                double r = MaxInRow(i, index);
                for (int j = 0; j < m_iSize; ++j) m_mVolatileA[i, j] /= r;
                m_vVolatileB[i] /= r;
                for (int k = i + 1; k < m_iSize; ++k)
                {
                    double p = m_mVolatileA[k, index[i]];
                    for (int j = i; j < m_iSize; ++j)
                        m_mVolatileA[k, index[j]] -= m_mVolatileA[i, index[j]] * p;
                    m_vVolatileB[k] -= m_vVolatileB[i] * p;
                    m_mVolatileA[k, index[i]] = 0.0;
                }
            }
        }

        private void BackwardStep(int[] index)
        {
            for (int i = m_iSize - 1; i >= 0; --i)
            {
                double x_i = m_vVolatileB[i];
                for (int j = i + 1; j < m_iSize; ++j)
                    x_i -= m_vX[index[j]] * m_mVolatileA[i, index[j]];
                m_vX[index[i]] = x_i;
            }
        }

        private void EstimateError()
        {
            for (int i = 0; i < m_iSize; ++i)
            {
                double actual_b_i = 0.0;
                for (int j = 0; j < m_iSize; ++j)
                    actual_b_i += m_mInitialA[i, j] * m_vX[j];
                m_vU[i] = m_vInitialB[i] - actual_b_i;
            }
        }
    }
}
