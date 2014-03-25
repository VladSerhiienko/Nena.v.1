using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HammingCode
{
    class BinaryMatrix
    {
        private bool[,] matrix;

        public int RowAmount { get { return matrix.GetLength(0); } }
        public int ColumnAmount { get { return matrix.GetLength(1); } }

        public BinaryMatrix(bool[,] matrix)
        {
            this.matrix = matrix;
        }

        public BinaryMatrix(int rowsAmount, int columsAmount)
        {
            this.matrix = new bool[rowsAmount,columsAmount];
        }

        public Binary GetRow (int index)
        {
            bool[] row = new bool[ColumnAmount];
            for (int x = 0; x < ColumnAmount; x++)
            {
                row[x] = matrix[index, x];
            }

            return new Binary(row);
        }

        public Binary GetColumn(int index)
        {
            bool[] column = new bool[RowAmount];
            for (int y = 0; y < RowAmount; y++)
            {
                column[y] = matrix[y, index];
            }

            return new Binary(column);
        }

        public void Set (int row, int column, bool value)
        {
            matrix[row, column] = value;
        }

        public override string ToString()
        {
            StringBuilder stringBuilder = new StringBuilder(matrix.Length);
            for (int y = 0; y < RowAmount; y++)
            {
                for (int x = 0; x < ColumnAmount; x++)
                {
                    stringBuilder.Append(matrix[y, x]?'1':'0');
                }
                stringBuilder.Append('\n');
            }
            return stringBuilder.ToString();
        }

        public bool Get(int row, int column)
        {
            return matrix[row, column];
        }
    }
}
