using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DavidonFletcherPowell
{
    public class Parameter
    {
        private bool _isSolvedFor = true;
        private double _value;
        private double _derivativeStep = 1e-3;
        private DerivativeStepType _derivativeStepType = DerivativeStepType.Relative;
        public Parameter() : base() { }
        public Parameter(double value) : this() { _value = value; }
        public Parameter(double value, double derivativeStep) : this(value) { _derivativeStep = derivativeStep; }
        public Parameter(double value, double derivativeStep, DerivativeStepType stepSizeType) : this(value, derivativeStep) { _derivativeStepType = stepSizeType; }
        public Parameter(double value, double derivativeStep, DerivativeStepType stepSizeType, bool isSolvedFor) : this(value, derivativeStep, stepSizeType) { _isSolvedFor = isSolvedFor; }
        public Parameter(bool isSolvedFor) : this() { _isSolvedFor = isSolvedFor; }
        public Parameter(Parameter clone) { _isSolvedFor = clone.IsSolvedFor; _value = clone.Value; _derivativeStep = clone.DerivativeStep; _derivativeStepType = clone.DerivativeStepType; }
        public bool IsSolvedFor { get { return _isSolvedFor; } set { _isSolvedFor = value; } }
        public double Value { get { return _value; } set { _value = value; } }
        public double DerivativeStep { get { return _derivativeStep; } set { _derivativeStep = value; } }
        public double DerivativeStepSize
        {
            get
            {
                double derivativeStepSize;
                if (_derivativeStepType == DerivativeStepType.Absolute)
                {
                    derivativeStepSize = _derivativeStep;
                }

                else
                {
                    if (_value != 0.0)
                    {
                        derivativeStepSize = _derivativeStep * Math.Abs(_value);
                    }
                    else { derivativeStepSize = _derivativeStep; }
                }
                return derivativeStepSize;
            }
        }
        public DerivativeStepType DerivativeStepType { get { return _derivativeStepType; } set { _derivativeStepType = value; } }
        public static implicit operator double(Parameter p) { return p.Value; }
        public override string ToString()
        {
            return "Parameter: Value:" + Value.ToString() +
                " IsSolvedFor:" + _isSolvedFor.ToString();
        }
    }

    public enum DerivativeStepType { Relative, Absolute }
    public class ParameterCollection : Collection<Parameter>
    {
        public ParameterCollection() : base() { }
    }

    public class Matrix{
        #region ctor
        public Matrix()
        {_values = new double[_rowCount, _columnCount];}
        public Matrix(int rowCount, int columnCount)
        
        {_rowCount = rowCount;_columnCount = columnCount;_values = new double[_rowCount, _columnCount];}
        #endregion
        #region Row Column values
        
        public double this[int row, int column]{
            get { return _values[row, column]; }
            set { _values[row, column] = value; }}
        
        #endregion
        #region F&P
        
        private double[,] _values;
        
        private int _rowCount = 3;
        public int RowCount{get { return _rowCount; }}
        private int _columnCount = 3;
        public int ColumnCount{get { return _columnCount; }}
        #endregion
        
        #region basic single matrix stuffpublic 
        
        static Matrix Identity(int size){
            Matrix resultMatrix = new Matrix(size, size);
            Parallel.For(0, size, (i) =>{for (int j = 0; j < size; j++){resultMatrix[i, j] = (i == j) ? 1.0 : 0.0;}});return resultMatrix;}
        
        public Matrix Clone(){Matrix resultMatrix = new Matrix(_rowCount, _columnCount);
            Parallel.For(0, _rowCount, (i) =>{for (int j = 0; j < _columnCount; j++)
            {resultMatrix[i, j] = this[i, j];}});return resultMatrix;}public Matrix Transpose()
            {Matrix resultMatrix = new Matrix(_columnCount, _rowCount);
                Parallel.For(0, _rowCount, (i) =>{for (int j = 0; j < _columnCount; j++)
                {resultMatrix[j, i] = this[i, j];}});return resultMatrix;}
        
        #endregion
        #region Binary Mathpublic 
        
        static Matrix Add(Matrix leftMatrix, Matrix rightMatrix)
        {
            Debug.Assert(leftMatrix.ColumnCount == rightMatrix.ColumnCount);
            Debug.Assert(leftMatrix.RowCount == rightMatrix.RowCount);
            Matrix resultMatrix = new Matrix(leftMatrix.RowCount, rightMatrix.ColumnCount);
            Parallel.For(0, leftMatrix.RowCount, (i) =>{
                for (int j = 0; j < leftMatrix.ColumnCount; j++){
                    resultMatrix[i, j] = leftMatrix[i, j] + rightMatrix[i, j];}});
            return resultMatrix;}public static Matrix operator +(Matrix leftMatrix, Matrix rightMatrix){
                return Matrix.Add(leftMatrix, rightMatrix);}
        
        public static Matrix Subtract(Matrix leftMatrix, Matrix rightMatrix)
        {Debug.Assert(leftMatrix.ColumnCount == rightMatrix.ColumnCount);
            Debug.Assert(leftMatrix.RowCount == rightMatrix.RowCount);
            Matrix resultMatrix = new Matrix(leftMatrix.RowCount, rightMatrix.ColumnCount);
            Parallel.For(0, leftMatrix.RowCount, (i) =>{for (int j = 0; j < leftMatrix.ColumnCount; j++){
                resultMatrix[i, j] = leftMatrix[i, j] - rightMatrix[i, j];}});return resultMatrix;}
        
        public static Matrix operator -(Matrix leftMatrix, Matrix rightMatrix){
            return Matrix.Subtract(leftMatrix, rightMatrix);}
        
        public static Matrix Multiply(Matrix leftMatrix, Matrix rightMatrix)
        {Debug.Assert(leftMatrix.ColumnCount == rightMatrix.RowCount);
            Matrix resultMatrix = new Matrix(leftMatrix.RowCount, rightMatrix.ColumnCount);
            Parallel.For(0, resultMatrix.ColumnCount, (i) =>{
                for (int j = 0; j < leftMatrix.RowCount; j++){
                    double value = 0.0;for (int k = 0; k < rightMatrix.RowCount; k++)
                    {value += leftMatrix[j, k] * rightMatrix[k, i];}resultMatrix[j, i] = value;}});
            return resultMatrix;}
        
        public static Matrix operator *(Matrix leftMatrix, Matrix rightMatrix){
            return Matrix.Multiply(leftMatrix, rightMatrix);}
        public static Matrix Multiply(double left, Matrix rightMatrix){
            Matrix resultMatrix = new Matrix(rightMatrix.RowCount, rightMatrix.ColumnCount);
            Parallel.For(0, resultMatrix.RowCount, (i) =>{for (int j = 0; j < rightMatrix.ColumnCount; j++){
                resultMatrix[i, j] = left * rightMatrix[i, j];}});return resultMatrix;}
        
        public static Matrix operator *(double left, Matrix rightMatrix){
            return Matrix.Multiply(left, rightMatrix);}
        
        public static Matrix Multiply(Matrix leftMatrix, double right){
            Matrix resultMatrix = new Matrix(leftMatrix.RowCount, leftMatrix.ColumnCount);
            Parallel.For(0, leftMatrix.RowCount, (i) =>{
                for (int j = 0; j < leftMatrix.ColumnCount; j++){
                    resultMatrix[i, j] = leftMatrix[i, j] * right;}});return resultMatrix;}
        
        public static Matrix operator *(Matrix leftMatrix, double right){
            return Matrix.Multiply(leftMatrix, right);}
        
        public static Matrix Divide(Matrix leftMatrix, double right){
            Matrix resultMatrix = new Matrix(leftMatrix.RowCount, leftMatrix.ColumnCount);
            Parallel.For(0, leftMatrix.RowCount, (i) =>{for (int j = 0; j < leftMatrix.ColumnCount; j++){
                resultMatrix[i, j] = leftMatrix[i, j] / right;}});return resultMatrix;}
        
        public static Matrix operator /(Matrix leftMatrix, double right){
            
            return Matrix.Divide(leftMatrix, right);}
        #endregion
        #region Assorted Castspublic 
        
        static Matrix FromArray(double[] left){
            int length = left.Length;Matrix resultMatrix = new Matrix(length, 1);
            for (int i = 0; i < length; i++){resultMatrix[i, 0] = left[i];}return resultMatrix;}
        public static implicit operator Matrix(double[] left){return FromArray(left);}
        public static double[] ToArray(Matrix leftMatrix){
            Debug.Assert((leftMatrix.ColumnCount == 1 && leftMatrix.RowCount >= 1) || 
                (leftMatrix.RowCount == 1 && leftMatrix.ColumnCount >= 1));
            
            double[] result = null;if (leftMatrix.ColumnCount > 1){
                int numElements = leftMatrix.ColumnCount;result = new double[numElements];
                for (int i = 0; i < numElements; i++){result[i] = leftMatrix[0, i];}}else{
                int numElements = leftMatrix.RowCount;result = new double[numElements];
                for (int i = 0; i < numElements; i++){result[i] = leftMatrix[i, 0];}}return result;}
        public static implicit operator double[](Matrix leftMatrix){return ToArray(leftMatrix);}
        public static Matrix FromDoubleArray(double[,] left){int length0 = left.GetLength(0);
            int length1 = left.GetLength(1);Matrix resultMatrix = new Matrix(length0, length1);
            for (int i = 0; i < length0; i++){for (int j = 0; j < length1; j++){
                resultMatrix[i, j] = left[i, j];}}return resultMatrix;}
        public static implicit operator Matrix(double[,] left){return FromDoubleArray(left);}
        public static double[,] ToDoubleArray(Matrix leftMatrix){
            double[,] result = new double[leftMatrix.RowCount, leftMatrix.ColumnCount];
            for (int i = 0; i < leftMatrix.RowCount; i++){
                for (int j = 0; j < leftMatrix.ColumnCount; j++){
                    result[i, j] = leftMatrix[i, j];}}return result;}
        
        public static implicit operator double[,](Matrix leftMatrix){
            
            return ToDoubleArray(leftMatrix);}
        
        #endregion
        
        public Matrix SolveFor(Matrix rightMatrix){
            Debug.Assert(rightMatrix.RowCount == _columnCount);
            Debug.Assert(_columnCount == _rowCount);
            Matrix resultMatrix = new Matrix(_columnCount, rightMatrix.ColumnCount);
            
            LUDecompositionResults resDecomp = LUDecompose();
            int[] nP = resDecomp.PivotArray;
            Matrix lMatrix = resDecomp.L;
            Matrix uMatrix = resDecomp.U;
            Parallel.For(0, rightMatrix.ColumnCount, k =>{
                //Solve for the corresponding d Matrix from Ld=Pb
                
                double sum = 0.0;Matrix dMatrix = new Matrix(_rowCount, 1);
                dMatrix[0, 0] = rightMatrix[nP[0], k] / lMatrix[0, 0];
                for (int i = 1; i < _rowCount; i++){sum = 0.0;
                    for (int j = 0; j < i; j++){sum += lMatrix[i, j] * dMatrix[j, 0];}
                    dMatrix[i, 0] = (rightMatrix[nP[i], k] - sum) / lMatrix[i, i];}
                //Solve for x using U
                
                //x = d
                //resultMatrix[_rowCount - 1, k] = dMatrix[_rowCount - 1, 0];
                for (int i = _rowCount - 2; i >= 0; i--){sum = 0.0;
                    for (int j = i + 1; j < _rowCount; j++){
                        sum += uMatrix[i, j] * resultMatrix[j, k];}
                    resultMatrix[i, k] = dMatrix[i, 0] - sum;}});
            return resultMatrix;}
        
        private LUDecompositionResults LUDecompose()
        {Debug.Assert(_columnCount == _rowCount);
            // Using Crout Decomp with P//// Ax = b 
            //By definition of problem variables.//
            // LU = PA //By definition of L, U, and P.//// LUx = Pb //By substition for PA.//// Ux = d //By definition of d//// Ld = Pb //By subsitition for d.////For 4x4 with P = I// [l11 0 0 0 ] [1 u12 u13 u14] [a11 a12 a13 a14]// [l21 l22 0 0 ] [0 1 u23 u24] = [a21 a22 a23 a24]// [l31 l32 l33 0 ] [0 0 1 u34] [a31 a32 a33 a34]// [l41 l42 l43 l44] [0 0 0 1 ] [a41 a42 a43 a44]LUDecompositionResults result = new LUDecompositionResults();try{int[] pivotArray = new int[_rowCount]; //Pivot matrix.Matrix uMatrix = new Matrix(_rowCount, _columnCount);Matrix lMatrix = new Matrix(_rowCount, _columnCount);Matrix workingUMatrix = Clone();Matrix workingLMatrix = new Matrix(_rowCount, _columnCount);Parallel.For(0, _rowCount, i =>{pivotArray[i] = i;});//Iterate down the number of rows in the U matrix.for (int i = 0; i < _rowCount; i++){//Do pivots first.//I want to make the matrix diagnolaly dominate.//Initialize the variables used to determine the pivot row.double maxRowRatio = double.NegativeInfinity;int maxRow = -1;int maxPosition = -1;//Check all of the rows below and including the current row//to determine which row should be pivoted to the working row position.//The pivot row will be set to the row with the maximum ratio//of the absolute value of the first column element divided by the//sum of the absolute values of the elements in that row.Parallel.For(i, _rowCount, j =>{//Store the sum of the absolute values of the row elements in//dRowSum. Clear it out now because I am checking a new row.double rowSum = 0.0;//Go across the columns, add the absolute values of the elements in//that column to dRowSum.for (int k = i; k < _columnCount; k++){rowSum += Math.Abs(workingUMatrix[pivotArray[j], k]);}//Check to see if the absolute value of the ratio of the lead//element over the sum of the absolute values of the elements is larger//that the ratio for preceding rows. If it is, then the current row//becomes the new pivot candidate.if (rowSum == 0.0){throw new SingularMatrixException();}double dCurrentRatio = Math.Abs(workingUMatrix[pivotArray[j], i]) / rowSum;lock (this){if (dCurrentRatio > maxRowRatio){maxRowRatio = Math.Abs(workingUMatrix[pivotArray[j], i] / rowSum);maxRow = pivotArray[j];maxPosition = j;}}});//If the pivot candidate isn't the current row, update the//pivot array to swap the current row with the pivot row.if (maxRow != pivotArray[i]){int hold = pivotArray[i];pivotArray[i] = maxRow;pivotArray[maxPosition] = hold;}//Store the value of the left most element in the working U//matrix in dRowFirstElementValue.double rowFirstElementValue = workingUMatrix[pivotArray[i], i];//Update the columns of the working row. j is the column index.Parallel.For(0, _columnCount, j =>{if (j < i){//If j<1, then the U matrix element value is 0.workingUMatrix[pivotArray[i], j] = 0.0;}else if (j == i){//If i == j, the L matrix value is the value of the//element in the working U matrix.workingLMatrix[pivotArray[i], j] = rowFirstElementValue;//The value of the U matrix for i == j is 1workingUMatrix[pivotArray[i], j] = 1.0;}else // j>i{//Divide each element in the current row of the U matrix by the//value of the first element in the rowworkingUMatrix[pivotArray[i], j] /= rowFirstElementValue;//The element value of the L matrix for j>i is 0workingLMatrix[pivotArray[i], j] = 0.0;}});//For the working U matrix, subtract the ratioed active row from the rows below it.//Update the columns of the rows below the working row. k is the row index.for (int k = i + 1; k < _rowCount; k++){//Store the value of the first element in the working row//of the U matrix.rowFirstElementValue = workingUMatrix[pivotArray[k], i];//Go accross the columns of row k.Parallel.For(0, _columnCount, j =>{if (j < i){//If j<1, then the U matrix element value is 0.workingUMatrix[pivotArray[k], j] = 0.0;}else if (j == i){//If i == j, the L matrix value is the value of the//element in the working U matrix.workingLMatrix[pivotArray[k], j] = rowFirstElementValue;//The element value of the L matrix for j>i is 0workingUMatrix[pivotArray[k], j] = 0.0;}else //j>i{workingUMatrix[pivotArray[k], j] = workingUMatrix[pivotArray[k], j] - rowFirstElementValue * workingUMatrix[pivotArray[i], j];}});}}Parallel.For(0, _rowCount, i =>{for (int j = 0; j < _rowCount; j++){uMatrix[i, j] = workingUMatrix[pivotArray[i], j];lMatrix[i, j] = workingLMatrix[pivotArray[i], j];}});result.U = uMatrix;result.L = lMatrix;result.PivotArray = pivotArray;}catch (AggregateException ex2){if (ex2.InnerExceptions.Count > 0){throw ex2.InnerExceptions[0];}else{throw ex2;}}catch (Exception ex3){throw ex3;}return result;}public Matrix Invert(){Debug.Assert(_rowCount == _columnCount);Matrix resultMatrix = SolveFor(Identity(_rowCount));Matrix matIdent = this * resultMatrix;return SolveFor(Identity(_rowCount));}}public class LUDecompositionResults{private Matrix _lMatrix;private Matrix _uMatrix;private int[] _pivotArray;public LUDecompositionResults(){}public LUDecompositionResults(Matrix matL, Matrix matU, int[] nPivotArray){_lMatrix = matL;_uMatrix = matU;_pivotArray = nPivotArray;}public Matrix L{get { return _lMatrix; }set { _lMatrix = value; }}public Matrix U{get { return _uMatrix; }set { _uMatrix = value; }}public int[] PivotArray{get { return _pivotArray; }set { _pivotArray = value; }}}public class SingularMatrixException : ArithmeticException{public SingularMatrixException(): base("Invalid operation on a singular matrix."){}}


    public class Polynomial{
        public static double[] SolvePolyCoefs(double[] x, double[] y)
        {
            Debug.Assert(x.Length > 0);
            Debug.Assert(x.Length == y.Length);
            Matrix m = new Matrix(x.Length, x.Length);
            Parallel.For(0, x.Length, i =>{m[i, 0] = 1.0;for (int j = 1; j < x.Length; j++)
            {m[i, j] = m[i, j - 1] * x[i];}});return m.SolveFor(y);}
        
        public static double ParabolicMin(double[] x, double[] y)
        {double result = double.NaN;Debug.Assert(x.Length == 3);
            Debug.Assert(y.Length == 3);double[] values = SolvePolyCoefs(x, y);
            if (values[2] > 0){result = -values[1] / (2.0 * values[2]);}return result;}
        public static CubicMinResults CubicMin(double[] x, double[] y){//0 is local min value X//1 is cubic value at min value X//2 is local max value X//3 is a status <0 means it failed.CubicMinResults result = new CubicMinResults();result.LocalMinX = 0.0;Debug.Assert(x.Length == 4);Debug.Assert(y.Length == 4);double[] values = SolvePolyCoefs(x, y);//  d is values[3]//  c is values[2]//  b is values[1]//  a is values[0]//  y=dx3+cx2+bx+a//  y'=3dx2+2cx+b which is equal to 0 at min and max.//  y''=6dx+2c which is greater than 0 at the min.//  Quad equation is (-b +- sqrt(b2-4ac))/2a//  Here, a=3d, b=2c, c=b which gives//  x = (-2c +- sqrt(4c2 - 12db))/6d//  Substituting x into y'' gives//  y''=6d((-2c +- sqrt(4c2 - 12db))/6d)+2c//  y''=-2c +- sqrt(4c2 - 12db) + 2c//  y''=+-sqrt(4c2 - 12db)//  y'' is positive at the local min, the sqrt is always positive, so the//  + solution is the local min and the - solution is the local max.//Make sure it is a cubic, i.e. d>0if (values[3] != 0.0 && (values[2] == 0 || Math.Abs(values[3] / values[2]) > 1e-10)){//Compute the square root term.double squareRoot = 4.0 * values[2] * values[2] - 12.0 * values[3] * values[1];if (double.IsNaN(squareRoot) || double.IsInfinity(squareRoot)) { Debugger.Break(); }//If the square root term is negative, then there are no real roots.if (squareRoot > 0){result.LocalMinX = (-2.0 * values[2] + Math.Sqrt(squareRoot)) / (6.0 * values[3]);result.LocalMaxX = (-2.0 * values[2] - Math.Sqrt(squareRoot)) / (6.0 * values[3]);result.ResultType = CubicResultType.StandardThirdOrder;}else{result.ResultType = CubicResultType.ThirdOrderImaginaryRoots;}}else //This is a second order, not a third.{if (values[2] > 0){result.LocalMinX = -values[1] / (2.0 * values[2]);result.ResultType = CubicResultType.StandardSecondOrder;}else{result.ResultType = CubicResultType.SecondOrderNegativeSecondDerivative;}}if (double.IsNaN(result.LocalMinX)){throw new ArithmeticException("Line search returned NaN");}return result;}public static double[] Regress(double[,] z, double[] y){//y=a0 z1 + a1 z1 +a2 z2 + a3 z3 +...//Z is the functional values.//Z index 0 is a row, the variables go across index 1.//Y is the summed value.//returns the coefficients.Debug.Assert(z != null && y != null);Debug.Assert(z.GetLength(0) == y.GetLength(0));Matrix zMatrix = z;Matrix zTransposeMatrix = zMatrix.Transpose();Matrix leftHandSide = zTransposeMatrix * zMatrix;Matrix rightHandSide = zTransposeMatrix * y;Matrix coefsMatrix = leftHandSide.SolveFor(rightHandSide);return coefsMatrix;}public static double[] RegressPolynomial(double[] x, double[] y, int polynomialOrder){double[,] z = new double[y.Length, polynomialOrder + 1];for (int i = 0; i < y.Length; i++){z[i, 0] = 1.0;for (int j = 1; j < polynomialOrder + 1; j++){z[i, j] = Math.Pow(x[i], j);}}double[] coefs = Polynomial.Regress(z, y);return coefs;}public static double EvaluatePolynomial(double[] coefs, double x){double result = coefs[0];for (int i = 1; i < coefs.Length; i++){result += coefs[i] * Math.Pow(x, i);}return result;}}public class CubicMinResults{private double _localMinX;private double _localMaxX;private CubicResultType _resultType;public CubicMinResults(){}public double LocalMinX{get { return _localMinX; }set { _localMinX = value; }}public double LocalMaxX{get { return _localMaxX; }set { _localMaxX = value; }}public CubicResultType ResultType{get { return _resultType; }set { _resultType = value; }}}public enum CubicResultType{StandardThirdOrder,ThirdOrderImaginaryRoots,StandardSecondOrder,SecondOrderNegativeSecondDerivative}


    public class RegionEliminationResults { private bool _succeeded = false; private double _xResult = double.NaN; private double _yResult = double.NaN; public RegionEliminationResults() { } public RegionEliminationResults(double result, double yResult) { _succeeded = true; _xResult = result; _yResult = yResult; } public RegionEliminationResults(bool succeeded) { _succeeded = succeeded; } public RegionEliminationResults(double xResult, double yResult, bool succeeded) { _succeeded = succeeded; _xResult = xResult; _yResult = yResult; } public bool Succeeded { get { return _succeeded; } set { _succeeded = value; } } public double XResult { get { return _xResult; } set { _xResult = value; } } public double YResult { get { return _yResult; } set { _yResult = value; } } }
    public class SearchPoint { private double _x = double.NaN; private double _y = double.NaN; public SearchPoint() { } public SearchPoint(double x) { _x = x; } public SearchPoint(double x, double y) { _x = x; _y = y; } public double X { get { return _x; } set { _x = value; } } public double Y { get { return _y; } set { _y = value; } } public override string ToString() { return "SearchPoint X:" + _x.ToString() + " Y:" + _y.ToString(); } }
    public class SearchPointCollection : Collection<SearchPoint> 
    { 
        private int _maxSize = 3; public SearchPointCollection() : base() { } 
        protected override void InsertItem(int index, SearchPoint item) { 
            while (index >= _maxSize) { 
                SearchPoint worstPoint = (from pt in this orderby pt.Y descending select pt).ToArray()[0]; 
                base.Remove(worstPoint); index--; } base.InsertItem(index, item); 
            SearchPoint[] points = (from pt in this orderby pt.X select pt).ToArray(); 
            for (int i = 0; i < points.Length; i++) { this[i] = points[i]; } } 
        
        public bool CanAdd(SearchPoint newPoint) { 
            bool canAdd = (from pt in this where pt.X == newPoint.X select pt).Count() == 0; 
            if (canAdd) { double currentMin = (from pt in this select pt.Y).Max(); 
                canAdd = newPoint.Y < currentMin; } return canAdd; } 
        
        public bool CanAdd(double newX) { return (from pt in this where pt.X == newX select pt).Count() == 0; }
        public bool IsBracketed() { double min = (from pt in this select pt.Y).Min(); 
            bool result = min < this[0].Y && min < this[Count - 1].Y; if (!result) { 
                double max = (from pt in this select pt.Y).Max(); result = min == max; } return result; } 
        
        public double ParabolicMinYEstimate() { double result = double.NaN; if (Count == 3) { 
            double[] x = (from pt in this select pt.X).ToArray(); 
            double[] y = (from pt in this select pt.Y).ToArray(); 
            result = Polynomial.ParabolicMin(x, y); } return result; 
        } 
        
        public double CubicMinYEstimate() { double result = double.NaN; if (Count == 4) { double[] x = (from pt in this select pt.X).ToArray(); double[] y = (from pt in this select pt.Y).ToArray(); CubicMinResults res = Polynomial.CubicMin(x, y); if (res.ResultType == CubicResultType.StandardSecondOrder || res.ResultType == CubicResultType.StandardThirdOrder) { result = res.LocalMinX; } } return result; } public int MaxSize { get { return _maxSize; } set { _maxSize = value; } } public SearchPointCollection Clone(int nMaxSize) { SearchPointCollection cloneCollection = new SearchPointCollection(); cloneCollection.MaxSize = nMaxSize; for (int i = 0; i < Count; i++) { cloneCollection.Add(new SearchPoint(this[i].X, this[i].Y)); } return cloneCollection; } public SearchPoint FindMinY() { return (from pt in this orderby pt.Y select pt).ToArray()[0]; } }
    public class RegionEliminationBase { protected bool _cancel = false; protected int _numStartingPoints = 2; public RegionEliminationBase() { } public virtual RegionEliminationResults RegionEliminate(Func<double, double> function, SearchPointCollection startPoint, double eliminationFraction) { return new RegionEliminationResults(); } public bool Cancel { get { bool result = false; lock (this) { result = _cancel; } return result; } set { lock (this) { _cancel = value; } } } protected void CalculateMissingValues(SearchPointCollection points, Func<double, double> function) { for (int i = 0; i < points.Count; i++) { if (double.IsNaN(points[i].Y)) { points[i].Y = function(points[i].X); } } } public int NumStartingPoints { get { return _numStartingPoints; } } }

    public class CubicRegionElimination : RegionEliminationBase
    {
        public CubicRegionElimination() : base() { _numStartingPoints = 4; }
        public override RegionEliminationResults RegionEliminate(
            Func<double, double> function, SearchPointCollection startPoint,
            double eliminationFraction)
        {
            Debug.Assert(startPoint.Count >= 2);
            bool succeeded = true;
            Cancel = false;
            SearchPointCollection searchPoints = startPoint.Clone(4);
            CalculateMissingValues(searchPoints, function);
            while (searchPoints.Count < 4) { double midPoint = (searchPoints[0].X + searchPoints[1].X) / 2.0; double functionValue = function(midPoint); searchPoints.Add(new SearchPoint(midPoint, functionValue)); }
            double stopRegionSize = (searchPoints[3].X - searchPoints[0].X) * eliminationFraction;
            while (!Cancel)
            {
                try
                {
                    double newX = searchPoints.CubicMinYEstimate();
                    if (!double.IsNaN(newX))
                    {
                        double functionValue = function(newX);
                        SearchPoint newPoint = new SearchPoint(newX, functionValue);
                        if (searchPoints.CanAdd(newPoint)) { searchPoints.Add(newPoint); }
                        else { Cancel = true; }
                    }
                    else { Cancel = true; succeeded = false; }
                    if (searchPoints[3].X - searchPoints[0].X < stopRegionSize) { Cancel = true; }
                }
                catch (SingularMatrixException) { Cancel = true; }
            }

            SearchPoint minimumPoint = searchPoints.FindMinY();
            return new RegionEliminationResults(minimumPoint.X, minimumPoint.Y, succeeded);
        }


        public class RegionEliminationResults { private bool _succeeded = false; private double _xResult = double.NaN; private double _yResult = double.NaN; public RegionEliminationResults() { } public RegionEliminationResults(double result, double yResult) { _succeeded = true; _xResult = result; _yResult = yResult; } public RegionEliminationResults(bool succeeded) { _succeeded = succeeded; } public RegionEliminationResults(double xResult, double yResult, bool succeeded) { _succeeded = succeeded; _xResult = xResult; _yResult = yResult; } public bool Succeeded { get { return _succeeded; } set { _succeeded = value; } } public double XResult { get { return _xResult; } set { _xResult = value; } } public double YResult { get { return _yResult; } set { _yResult = value; } } }
        public class SearchPoint { private double _x = double.NaN; private double _y = double.NaN; public SearchPoint() { } public SearchPoint(double x) { _x = x; } public SearchPoint(double x, double y) { _x = x; _y = y; } public double X { get { return _x; } set { _x = value; } } public double Y { get { return _y; } set { _y = value; } } public override string ToString() { return "SearchPoint X:" + _x.ToString() + " Y:" + _y.ToString(); } }
        public class SearchPointCollection : Collection<SearchPoint> { private int _maxSize = 3; public SearchPointCollection() : base() { } protected override void InsertItem(int index, SearchPoint item) { while (index >= _maxSize) { SearchPoint worstPoint = (from pt in this orderby pt.Y descending select pt).ToArray()[0]; base.Remove(worstPoint); index--; } base.InsertItem(index, item); SearchPoint[] points = (from pt in this orderby pt.X select pt).ToArray(); for (int i = 0; i < points.Length; i++) { this[i] = points[i]; } } public bool CanAdd(SearchPoint newPoint) { bool canAdd = (from pt in this where pt.X == newPoint.X select pt).Count() == 0; if (canAdd) { double currentMin = (from pt in this select pt.Y).Max(); canAdd = newPoint.Y < currentMin; } return canAdd; } public bool CanAdd(double newX) { return (from pt in this where pt.X == newX select pt).Count() == 0; } public bool IsBracketed() { double min = (from pt in this select pt.Y).Min(); bool result = min < this[0].Y && min < this[Count - 1].Y; if (!result) { double max = (from pt in this select pt.Y).Max(); result = min == max; } return result; } public double ParabolicMinYEstimate() { double result = double.NaN; if (Count == 3) { double[] x = (from pt in this select pt.X).ToArray(); double[] y = (from pt in this select pt.Y).ToArray(); result = Polynomial.ParabolicMin(x, y); } return result; } public double CubicMinYEstimate() { double result = double.NaN; if (Count == 4) { double[] x = (from pt in this select pt.X).ToArray(); double[] y = (from pt in this select pt.Y).ToArray(); CubicMinResults res = Polynomial.CubicMin(x, y); if (res.ResultType == CubicResultType.StandardSecondOrder || res.ResultType == CubicResultType.StandardThirdOrder) { result = res.LocalMinX; } } return result; } public int MaxSize { get { return _maxSize; } set { _maxSize = value; } } public SearchPointCollection Clone(int nMaxSize) { SearchPointCollection cloneCollection = new SearchPointCollection(); cloneCollection.MaxSize = nMaxSize; for (int i = 0; i < Count; i++) { cloneCollection.Add(new SearchPoint(this[i].X, this[i].Y)); } return cloneCollection; } public SearchPoint FindMinY() { return (from pt in this orderby pt.Y select pt).ToArray()[0]; } }
        public class RegionEliminationBase { protected bool _cancel = false; protected int _numStartingPoints = 2; public RegionEliminationBase() { } public virtual RegionEliminationResults RegionEliminate(Func<double, double> function, SearchPointCollection startPoint, double eliminationFraction) { return new RegionEliminationResults(); } public bool Cancel { get { bool result = false; lock (this) { result = _cancel; } return result; } set { lock (this) { _cancel = value; } } } protected void CalculateMissingValues(SearchPointCollection points, Func<double, double> function) { for (int i = 0; i < points.Count; i++) { if (double.IsNaN(points[i].Y)) { points[i].Y = function(points[i].X); } } } public int NumStartingPoints { get { return _numStartingPoints; } } }

        public class BracketingBase
        {
            public BracketingBase() { }
            public virtual SearchPointCollection Bracket(Func<double, double> function, double startPosition, double step, int targetReturnPoints) { throw new NotImplementedException(); }public virtual SearchPointCollection Bracket(Func<double, double> function, double startPosition, double step) { return Bracket(function, startPosition, step, 2); }protected bool _cancel = false; public bool Cancel { get { bool result = false; lock (this) { result = _cancel; } return result; } set { lock (this) { _cancel = value; } } }
        }

        public class CubicBracketing : BracketingBase
        {
            public CubicBracketing() : base() { }
            public override SearchPointCollection Bracket(
                Func<double, double> function,
                double startPosition,
                double step,
                int targetReturnPoints
                )
            {
                SearchPointCollection returnCollection = new SearchPointCollection();
                //returnCollection.MaxSize = targetReturnPoints;
                returnCollection.MaxSize = 4;
                double delta = step;
                double currentPosition = startPosition;
                double functionValue = function(currentPosition);
                SearchPoint newSearchPoint = new SearchPoint(currentPosition, functionValue);
                returnCollection.Add(newSearchPoint); currentPosition += delta;
                functionValue = function(currentPosition);
                while (double.IsNaN(functionValue) || double.IsInfinity(functionValue))
                {
                    delta /= 10.0;
                    currentPosition = startPosition + delta;
                    functionValue = function(currentPosition);
                }
                if (functionValue > newSearchPoint.Y)
                {
                    newSearchPoint = new SearchPoint(currentPosition, functionValue);
                    returnCollection.Add(newSearchPoint); delta *= -1.0;
                    currentPosition = startPosition + delta;
                    functionValue = function(currentPosition);
                    newSearchPoint = new SearchPoint(currentPosition, functionValue);
                    returnCollection.Add(newSearchPoint);
                }
                else
                {
                    newSearchPoint = new SearchPoint(currentPosition, functionValue);
                    returnCollection.Add(newSearchPoint);
                }

                //Get third point.

                currentPosition += delta;
                functionValue = function(currentPosition);
                newSearchPoint = new SearchPoint(currentPosition, functionValue);
                returnCollection.Add(newSearchPoint);

                //If not already bracketed, try quadratic.

                double offsetDistanceFromEstMin = 0.1;
                if (!returnCollection.IsBracketed() && !Cancel)
                {
                    double newX = returnCollection.ParabolicMinYEstimate();
                    if (!double.IsNaN(newX))
                    {
                        if (newX > returnCollection[2].X)
                        {
                            newX += offsetDistanceFromEstMin * (newX - returnCollection[2].X);
                            functionValue = function(newX);
                            newSearchPoint = new SearchPoint(newX, functionValue);
                            returnCollection.Add(newSearchPoint);
                        }
                        else if (newX < returnCollection[0].X)
                        {
                            newX += offsetDistanceFromEstMin * (newX - returnCollection[0].X);
                            functionValue = function(newX); newSearchPoint = new SearchPoint(newX, functionValue);
                            returnCollection.Add(newSearchPoint);
                        }
                        else
                        {
                            //The point is inside the bracket.

                            if ((newX - returnCollection[0].X) < (returnCollection[2].X - newX))
                            {
                                newX = returnCollection[0].X - Math.Abs(delta);
                                functionValue = function(newX);
                                newSearchPoint = new SearchPoint(newX, functionValue);
                                returnCollection.Add(newSearchPoint);
                            }
                            else
                            {
                                newX = returnCollection[2].X + Math.Abs(delta);
                                functionValue = function(newX);
                                newSearchPoint = new SearchPoint(newX, functionValue);
                                returnCollection.Add(newSearchPoint);
                            }
                        }
                        currentPosition = newX;
                    }
                    else
                    {
                        delta *= 2.0;
                        currentPosition += delta;
                        functionValue = function(currentPosition);
                        newSearchPoint = new SearchPoint(currentPosition, functionValue);
                        returnCollection.Add(newSearchPoint);
                    }
                    while (!returnCollection.IsBracketed() && !Cancel)
                    {
                        newX = returnCollection.CubicMinYEstimate();
                        if (!double.IsNaN(newX))
                        {
                            if (newX > returnCollection[3].X)
                            {
                                newX += offsetDistanceFromEstMin * (newX - returnCollection[3].X);
                                functionValue = function(newX);
                                newSearchPoint = new SearchPoint(newX, functionValue);
                                returnCollection.Add(newSearchPoint);
                            }
                            else if (newX < returnCollection[0].X)
                            {
                                newX += offsetDistanceFromEstMin * (newX - returnCollection[0].X);
                                functionValue = function(newX);
                                newSearchPoint = new SearchPoint(newX, functionValue);
                                returnCollection.Add(newSearchPoint);
                            }
                            else
                            {
                                //The point is inside the bracket.

                                if ((newX - returnCollection[0].X) < (returnCollection[3].X - newX))
                                {
                                    newX = returnCollection[0].X - Math.Abs(delta);
                                    functionValue = function(newX);
                                    newSearchPoint = new SearchPoint(newX, functionValue);
                                    returnCollection.Add(newSearchPoint);
                                }
                                else
                                {
                                    newX = returnCollection[3].X + Math.Abs(delta);
                                    functionValue = function(newX);
                                    newSearchPoint = new SearchPoint(newX, functionValue);
                                    returnCollection.Add(newSearchPoint);
                                }
                            } currentPosition = newX;
                        }
                        else
                        {
                            delta *= 2.0;
                            currentPosition += delta;
                            functionValue = function(currentPosition);
                            newSearchPoint = new SearchPoint(currentPosition, functionValue);
                            returnCollection.Add(newSearchPoint);
                        }
                    }
                } return returnCollection;
            }

        }

        public class Derivatives
        {
            // _coefficients is the array of differential coefficients matrices.
            // The index corresponds to the position from the left edge
            // of the points.
            // I.e _coefficients[0] is for a matrix with three points in it corresponds to
            // the left most point.
            // The coefficients of the derivatives go down by row.  I.e. the first row
            // is the functional value, the second row is for the first derivative of the functional
            // value, the third row is the second derivative of the functional value.
            // The columns correspond to the points themselves.

            private Matrix[] _coefficients; private Derivatives() { }
            public Derivatives(int numberOfPoints) : this() { SolveCoefs(numberOfPoints); }
            public void SolveCoefs(int numberOfPoints) { _coefficients = new Matrix[numberOfPoints]; for (int i = 0; i < numberOfPoints; i++) { Matrix deltsMatrix = new Matrix(numberOfPoints, numberOfPoints); for (int j = 0; j < numberOfPoints; j++) { double delt = (double)(j - i); double HTerm = 1.0; for (int k = 0; k < numberOfPoints; k++) { deltsMatrix[j, k] = HTerm / Factorial(k); HTerm *= delt; } } _coefficients[i] = deltsMatrix.Invert(); double numPointsFactorial = Factorial(numberOfPoints); for (int j = 0; j < numberOfPoints; j++) { for (int k = 0; k < numberOfPoints; k++) { _coefficients[i][j, k] = (Math.Round(_coefficients[i][j, k] * numPointsFactorial)) / numPointsFactorial; } } } }private static double Factorial(int value) { double result = 1.0; for (int i = 1; i <= value; i++) { result *= (double)i; } return result; }

            /// <summary>/// Computes the derivative of a function.
            /// </summary>/// <param name="points">Equally spaced function value points</param>
            /// <param name="order">The order of the derivative to take</param>
            /// <param name="variablePosition">The position in the array of function values to take the derivative at.</param>
            /// <param name="step">The x axis step size.</param>
            /// <returns></returns>
            public double ComputeDerivative(double[] points, int order, int variablePosition, double step) { Debug.Assert(points.Length == _coefficients.Length); Debug.Assert(order < _coefficients.Length); double result = 0.0; for (int i = 0; i < _coefficients.Length; i++) { result += _coefficients[variablePosition][order, i] * points[i]; } result /= Math.Pow(step, order); return result; }public double ComputePartialDerivative(Func<double> function, Parameter parameter, int order) { int numberOfPoints = _coefficients.Length; double result = 0.0; double originalValue = parameter; double[] points = new double[numberOfPoints]; double derivativeStepSize = parameter.DerivativeStepSize; int centerPoint = (numberOfPoints - 1) / 2; for (int i = 0; i < numberOfPoints; i++) { parameter.Value = originalValue + ((double)(i - centerPoint)) * derivativeStepSize; points[i] = function(); } result = ComputeDerivative(points, order, centerPoint, derivativeStepSize); parameter.Value = originalValue; return result; }public double ComputePartialDerivative(Func<double> function, Parameter parameter, int order, double currentFunctionValue) { int numberOfPoints = _coefficients.Length; double result = 0.0; double originalValue = parameter; double[] points = new double[numberOfPoints]; double derivativeStepSize = parameter.DerivativeStepSize; int centerPoint = (numberOfPoints - 1) / 2; for (int i = 0; i < numberOfPoints; i++) { if (i != centerPoint) { parameter.Value = originalValue + ((double)(i - centerPoint)) * derivativeStepSize; points[i] = function(); } else { points[i] = currentFunctionValue; } } result = ComputeDerivative(points, order, centerPoint, derivativeStepSize); parameter.Value = originalValue; return result; }public double[] ComputePartialDerivatives(Func<double> function, Parameter parameter, int[] derivativeOrders) { int numberOfPoints = _coefficients.Length; double[] result = new double[derivativeOrders.Length]; double originalValue = parameter; double[] points = new double[numberOfPoints]; double derivativeStepSize = parameter.DerivativeStepSize; int centerPoint = (numberOfPoints - 1) / 2; for (int i = 0; i < numberOfPoints; i++) { parameter.Value = originalValue + ((double)(i - centerPoint)) * derivativeStepSize; points[i] = function(); } for (int i = 0; i < derivativeOrders.Length; i++) { result[i] = ComputeDerivative(points, derivativeOrders[i], centerPoint, derivativeStepSize); } parameter.Value = originalValue; return result; }public double[] ComputePartialDerivatives(Func<double> function, Parameter parameter, int[] derivativeOrders, double currentFunctionValue) { int numberOfPoints = _coefficients.Length; double[] result = new double[derivativeOrders.Length]; double originalValue = parameter; double[] points = new double[numberOfPoints]; double derivativeStepSize = parameter.DerivativeStepSize; int centerPoint = (numberOfPoints - 1) / 2; for (int i = 0; i < numberOfPoints; i++) { if (i != centerPoint) { parameter.Value = originalValue + ((double)(i - centerPoint)) * derivativeStepSize; points[i] = function(); } else { points[i] = currentFunctionValue; } } for (int i = 0; i < derivativeOrders.Length; i++) { result[i] = ComputeDerivative(points, derivativeOrders[i], centerPoint, derivativeStepSize); } parameter.Value = originalValue; return result; }
        }


        public class QuasiNewtonBase
        {
            protected BracketingBase _bracketingMethod = new CubicBracketing();
            protected RegionEliminationBase _regionEliminationMethod = new CubicRegionElimination();
            protected Matrix _inverseHessian; protected Parameter[] _solvedForParameters;
            protected Func<double> _optimizationFunction;
            protected Derivatives _derivatives; protected double[] _currentGradient;
            protected double[] _lastGradient; protected double[] _searchDirection;
            protected double[] _currentPosition; protected double[] _lastPosition;
            protected double _regionEliminationFraction = 1e-5;
            protected int _iterationsSinceReset = 0;
            protected double _functionValue;
            protected int _inverseHessianResetCount = -1;
            public QuasiNewtonBase(Func<double> optimizationFunction, Parameter[] optimizationParameters, double inverseHessianResetMultiple, int numberOfDerivativePoints)
            {
                _solvedForParameters = optimizationParameters;
                _optimizationFunction = optimizationFunction;
                _derivatives = new Derivatives(numberOfDerivativePoints);
                int numberOfParameters = _solvedForParameters.Length;
                _inverseHessian = new Matrix(numberOfParameters, numberOfParameters);
                ResetInverseHessian();
                _inverseHessianResetCount = (int)(inverseHessianResetMultiple *
                    (double)_solvedForParameters.Length);
            }

            public QuasiNewtonBase(Func<double> optimizationFunction, Parameter[] optimizationParameters) :
                this(optimizationFunction, optimizationParameters, 2.0, 3) { }

            public bool Iterate()
            {
                int numberOfParameters = _solvedForParameters.Length;
                _currentPosition = (from d in _solvedForParameters select d.Value).ToArray();

                CalculateGradient();

                if (_inverseHessianResetCount > 0 && _iterationsSinceReset > _inverseHessianResetCount) { ResetInverseHessian(); }
                if (_iterationsSinceReset > 0)
                {
                    double dMoveDistance = 0.0;
                    for (int i = 0; i < numberOfParameters; i++) { double dDistDiff = _currentPosition[i] - _lastPosition[i]; dMoveDistance += dDistDiff * dDistDiff; }
                    if (dMoveDistance == 0.0) { ResetInverseHessian(); }
                    else { UpdateInverseHessian(); }
                }

                CalculateSearchDirection();

                bool succeeded = (from d in _searchDirection where double.IsNaN(d) select d).Count() == 0;
                if (succeeded)
                {
                    SearchPointCollection colSearch = _bracketingMethod.Bracket(LineSearchFunction, 0.0, 1.0, _regionEliminationMethod.NumStartingPoints);
                    RegionEliminationResults resRegionEliminate = _regionEliminationMethod.RegionEliminate(LineSearchFunction, colSearch, _regionEliminationFraction);
                    if (resRegionEliminate.Succeeded)
                    {
                        _lastGradient = _currentGradient; _lastPosition = _currentPosition; MoveParametersToPosition(resRegionEliminate.XResult);
                        _functionValue = resRegionEliminate.YResult;
                    } _iterationsSinceReset++;
                }
                else { Debugger.Break(); } return succeeded;
            }protected virtual void UpdateInverseHessian() { throw new NotImplementedException(); }
            protected double LineSearchFunction(double length)
            {
                MoveParametersToPosition(length);
                return _optimizationFunction();
            }protected void MoveParametersToPosition(double length) { int numberOfParameters = _solvedForParameters.Length; for (int i = 0; i < numberOfParameters; i++) { _solvedForParameters[i].Value = _currentPosition[i] + length * _searchDirection[i]; } }protected void CalculateSearchDirection() { _searchDirection = -1.0 * _inverseHessian * _currentGradient; }public void ResetInverseHessian() { int numberOfParameters = _solvedForParameters.Length; for (int i = 0; i < numberOfParameters; i++) { for (int j = 0; j < numberOfParameters; j++) { if (i == j) { double dSecondDeriv = _derivatives.ComputePartialDerivative(_optimizationFunction, _solvedForParameters[i], 2); if (dSecondDeriv != 0.0) { _inverseHessian[i, j] = Math.Abs(1.0 / dSecondDeriv); } else { _inverseHessian[i, j] = 1.0; } } else { _inverseHessian[i, j] = 0.0; } } } _iterationsSinceReset = 0; }


            // xgT
            protected Matrix ComputeQNProduct(double[] x, double[] g)
            {
                System.Diagnostics.Debug.Assert(x.Length == g.Length);
                Matrix mRet = new Matrix(x.Length, x.Length);
                for (int i = 0; i < x.Length; i++)
                {
                    for (int j = 0; j < x.Length; j++) { mRet[i, j] = x[i] * g[j]; }
                }
                return mRet;
            }

            protected double ComputeInnerProduct(double[] left, double[] right)
            {
                System.Diagnostics.Debug.Assert(left.Length == right.Length);
                double result = 0.0; for (int i = 0; i < left.Length; i++) { result += left[i] * right[i]; }
                return result;
            }

            protected void CalculateGradient()
            {
                int numberOfParameters = _solvedForParameters.Length;

                _currentGradient = new double[numberOfParameters];
                for (int i = 0; i < numberOfParameters; i++)
                {
                    _currentGradient[i] = _derivatives.ComputePartialDerivative(_optimizationFunction, _solvedForParameters[i], 1);
                }
            }

            public BracketingBase BracketingMethod { get { return _bracketingMethod; } set { _bracketingMethod = value; } }
            public RegionEliminationBase RegionEliminationMethod
            {
                get { return _regionEliminationMethod; }
                set { _regionEliminationMethod = value; }
            }

            public double RegionEliminationFraction
            {
                get { return _regionEliminationFraction; }
                set { _regionEliminationFraction = value; }
            }

            public double FunctionValue { get { return _functionValue; } }

        }

        public class DFP : QuasiNewtonBase
        {
            public DFP(Func<double> function,
                Parameter[] optimizationParameters,
                double inverseHessianResetMultiple,
                int numberOfDerivativePoints) :
                base(
                function,
                optimizationParameters,
                inverseHessianResetMultiple,
                numberOfDerivativePoints)
            { }

            public DFP(Func<double> function, Parameter[] optimizationParameters) :
                this(function, optimizationParameters, 2.0, 3) { }
            protected override void UpdateInverseHessian()
            {
                int numberOfParameters = _solvedForParameters.Length;
                double[] positionDelta = new double[numberOfParameters];
                double[] gradientDelta = new double[numberOfParameters];
                for (int i = 0; i < numberOfParameters; i++)
                { gradientDelta[i] = _currentGradient[i] - _lastGradient[i]; positionDelta[i] = _currentPosition[i] - _lastPosition[i]; } Matrix XXt = ComputeQNProduct(positionDelta, positionDelta); double YtX = ComputeInnerProduct(gradientDelta, positionDelta); Matrix T1 = XXt / YtX; Matrix YYt = ComputeQNProduct(gradientDelta, gradientDelta); Matrix AYYtAt = _inverseHessian * YYt * _inverseHessian.Transpose(); double YtAY = ComputeInnerProduct(gradientDelta, _inverseHessian * gradientDelta); Matrix T2 = AYYtAt / YtAY; Matrix matInverseHessianNew = _inverseHessian + T1 - T2; _inverseHessian = matInverseHessianNew;
            }
        }

    }
}
