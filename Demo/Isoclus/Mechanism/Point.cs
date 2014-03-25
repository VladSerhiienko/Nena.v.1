namespace Isoclus.Mechanism
{
    using System;
    using System.Collections.Generic;

    /// <summary>
    /// AssociatedPoints element
    /// </summary>
    class Point : ICloneable
    {
        /// <summary>
        ///  Unique name
        /// </summary>
        public string Id;

        /// <summary>
        ///  X position
        /// </summary>
        public float X;
        /// <summary>
        ///  Y position
        /// </summary>
        public float Y;

        /// <summary>
        /// Distance to the point from the central coords (0, 0)
        /// </summary>
        public float Length
        {
            get
            {
                return (float)Math.Sqrt(X * X + Y * Y);
            }
        }

        public float MaxCoord(out int Direction)
        {
            if (X > Y) 
            { 
                Direction = 1;
                return X;
            }
            else
            {
                Direction = 2;
                return Y;
            }
        }

        /// <summary>
        /// Initializes point
        /// </summary>
        public Point(
            float XPosition = .0f, float YPosition = .0f, 
            int Index = 0, bool Special = false
            )
        {
            X = XPosition;
            Y = YPosition;

            if (!Special)
                // Assign cluster id
                Id = Index == 0
                    ? "X" + NextId
                    : "X" + Index;

            // Correct next cluster index
            if (Index != NextId && Index != 0)
            {
                NextId = Index;
            }

            if (!Special)
                NextId++;
        }

        public object Clone()
        {
            var Result = new Point(Special: true);
            Result.Id = Id;
            Result.X = X;
            Result.Y = Y;
            return Result;
        }

        public override string ToString()
        {
            return ("Point[" + Id + " (" + X + "," + Y + ")]");
        }

        public static Point operator +(Point Point1, Point Point2)
        {
            return new Point
            {
                X = Point1.X + Point2.X,
                Y = Point1.Y + Point2.Y
            };
        }

        public static Point operator -(Point Point1, Point Point2)
        {
            return new Point
            {
                X = Point1.X - Point2.X,
                Y = Point1.Y - Point2.Y
            };
        }

        public static Point operator -(Point Point)
        {
            return new Point
            {
                X = -Point.X,
                Y = -Point.Y
            };
        }

        public static Point operator *(Point Point, float Value)
        {
            return new Point
            {
                X = Point.X * Value,
                Y = Point.Y * Value
            };
        }

        public static Point operator /(Point Point, float Value)
        {
            return new Point
            {
                X = Point.X / Value,
                Y = Point.Y / Value
            };
        }

        static public int NextId
        {
            get;
            private set;
        }

        static Point()
        {
            NextId = 1;
        }
    }

    static class PointExtensions
    {
        public static Point Sqrt(this Point Point)
        {
            return Point.Power(0.5f);
        }

        public static Point Sqr(this Point Point)
        {
            return Point.Power(2.0f);
        }

        public static Point Power(this Point Point, float Power)
        {
            Point Result = Point;
            Result.X = (float)Math.Pow(Point.X, Power);
            Result.Y = (float)Math.Pow(Point.Y, Power);
            return Result;
        }
    }
}