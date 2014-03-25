using System;
using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;

namespace SmallTalk.Labwork.Analysis.Utility
{
    /// <summary>
    /// Stores a full specification of combinations to generate, and allows the construction of enumerators to generate the combinations.
    /// </summary>
    /// <typeparam name="T">the type to generate combinations of</typeparam>
    public class CombinationGenerator<T> : IEnumerable<T[]>
    {
        private ElementSet<T> _Elements;
        private int _Choose;

        /// <summary>
        /// Constructs a new <c>Combinations</c> given a collection of elements and how many elements should be present in each combination. Note that if the same collection of elements is used with multiple <paramref name="choose"/> values, it is faster to construct an <see cref="ElementSet{T}"/> once and use <see cref="CombinationGenerator{T}(ElementSet{CombinationGenerator}, int)"/>.
        /// </summary>
        /// <param name="elements">the collection of elements</param>
        /// <param name="choose">the length of a combination</param>
        public CombinationGenerator(IEnumerable<T> elements, int choose)
            : this(new ElementSet<T>(elements, null), choose)
        {
        }

        /// <summary>
        /// Constructs a new <c>Combinations</c> given a collection of elements and how many elements should be present in each combination. Note that if the same collection of elements is used with multiple <paramref name="choose"/> values, it is faster to construct an <see cref="ElementSet{T}"/> once and use <see cref="CombinationGenerator{T}(ElementSet{CombinationGenerator}, int)"/>.
        /// </summary>
        /// <param name="elements">the collection of elements</param>
        /// <param name="choose">the length of a combination</param>
        /// <param name="comparer">the comparer used to order the elements</param>
        public CombinationGenerator(IEnumerable<T> elements, IComparer<T> comparer, int choose)
            : this(new ElementSet<T>(elements, comparer), choose)
        {
        }

        /// <summary>
        /// Constructs a new <c>Combinations</c> given a collection of elements and how many elements should be present in each combination. If the same set of elements will be used with different values of <paramref name="choose"/>, this constructor is the best choice as the elements are processed only once, during the construction of the <see cref="ElementSet{T}"/>.
        /// </summary>
        /// <param name="elements">the collection of elements</param>
        /// <param name="choose">the length of a combination</param>
        public CombinationGenerator(ElementSet<T> elements, int choose)
        {
            if (elements == null)
            {
                throw new ArgumentNullException("elements");
            }
            if (choose < 0 || choose > elements.Count)
            {
                throw new ArgumentOutOfRangeException("choose");
            }
            _Elements = elements;
            _Choose = choose;
        }

        /// <summary>
        /// Creates and returns an enumerator over the combinations. Each combination's elements will be returned in ascending order as defined by the compararer. The combinations themselves will also be returned in ascending order.
        /// </summary>
        /// <returns>an enumerator</returns>
        /// <seealso cref="IEnumerable{T}.GetEnumerator"/>
        public IEnumerator<T[]> GetEnumerator()
        {
            if (_Choose == 0)
            {
                return new ChooseZeroEnumerator();
            }
            else
            {
                return new Enumerator(this);
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return new Enumerator(this);
        }

        private class Enumerator : IEnumerator<T[]>
        {
            private CombinationGenerator<T> _Combinations;
            private int[] _Indices, _IndicesAt;
            private bool _PastEnd;

            internal Enumerator(CombinationGenerator<T> combinations)
            {
                _Combinations = combinations;
                _Indices = new int[_Combinations._Choose];
                _IndicesAt = new int[_Combinations._Elements.Elements.Count];
                Reset();
            }

            private void MoveFirst()
            {
                // Move all the indices as far left as possible.
                // Lower-numbered indices go further left.

                int currentIndex = 0;
                int usedCurrent = 0;
                for (int i = 0; i < _Indices.Length; i++)
                {
                    Debug.Assert(currentIndex < _Combinations._Elements.Elements.Count);
                    _Indices[i] = currentIndex;
                    _IndicesAt[currentIndex]++;
                    usedCurrent++;
                    if (usedCurrent == _Combinations._Elements.Elements.Values[currentIndex])
                    {
                        // We've used all of the current element. Go to the next element.
                        currentIndex++;
                        usedCurrent = 0;
                    }
                }
            }

            private void MoveIndex(int index, int offset)
            {
                if (_Indices[index] >= 0 && _Indices[index] < _IndicesAt.Length)
                {
                    _IndicesAt[_Indices[index]]--;
                }
                _Indices[index] += offset;
                if (_Indices[index] >= 0 && _Indices[index] < _IndicesAt.Length)
                {
                    _IndicesAt[_Indices[index]]++;
                }
            }

            private bool IsFull(int position)
            {
                // True if (position) has as many indices as it can hold.
                return _IndicesAt[position] == _Combinations._Elements.Elements.Values[position];
            }

            private bool CanFitRemainingIndices(int index)
            {
                int space = _Combinations._Elements.ElementsAtOrAfter[_Indices[index]];
                return space >= _Indices.Length - index;
            }

            private bool AdvanceIndex(int index, int doNotReach)
            {
                // First move the index one position to the right.
                MoveIndex(index, 1);

                // If we've found an existing position with no other index, and there's room at-and-to-the-right-of us for all the indices greater than us, we're good.
                if (_Indices[index] < doNotReach)
                {
                    if (_IndicesAt[_Indices[index]] == 1)
                    {
                        if (CanFitRemainingIndices(index))
                        {
                            return true;
                        }
                    }
                }

                // We've either fallen off the right hand edge, or hit a position with another index. If we're index 0, we're past the end of the enumeration. If not, we need to advance the next lower index, then push ourself left as far as possible until we hit another occupied position.
                if (index == 0)
                {
                    _PastEnd = true;
                    return false;
                }
                if (!AdvanceIndex(index - 1, _Indices[index]))
                {
                    return false;
                }

                // We must move at least one space to the left. If we can't, the enumeration is over.
                // If the position immediately to the left is already full, we can't move there.
                if (IsFull(_Indices[index] - 1))
                {
                    _PastEnd = true;
                    return false;
                }

                // Move left until the next leftmost element is full, or the current element has an index.
                do
                {
                    MoveIndex(index, -1);
                } while (_IndicesAt[_Indices[index]] == 1 && !IsFull(_Indices[index] - 1));
                return true;
            }

            public T[] Current
            {
                get
                {
                    if (_Indices[0] == -1 || _PastEnd)
                    {
                        // Before first or after last.
                        throw new InvalidOperationException();
                    }
                    else
                    {
                        T[] current = new T[_Indices.Length];
                        for (int i = 0; i < _Indices.Length; i++)
                        {
                            current[i] = _Combinations._Elements.Elements.Keys[_Indices[i]];
                        }
                        return current;
                    }
                }
            }

            public void Dispose()
            {
                // Do nothing.
            }

            object IEnumerator.Current
            {
                get
                {
                    return Current;
                }
            }

            public bool MoveNext()
            {
                if (_PastEnd)
                {
                    return false;
                }
                if (_Indices[0] == -1)
                {
                    MoveFirst();
                    return true;
                }
                else
                {
                    bool ret = AdvanceIndex(_Indices.Length - 1, _IndicesAt.Length);
                    return ret;
                }
            }

            public void Reset()
            {
                for (int i = 0; i < _Indices.Length; i++)
                {
                    _Indices[i] = -1;
                }
                Array.Clear(_IndicesAt, 0, _IndicesAt.Length);
                _PastEnd = false;
            }
        }

        private class ChooseZeroEnumerator : IEnumerator<T[]>
        {
            private enum State
            {
                BeforeBeginning,
                OnElement,
                PastEnd,
            }

            private State _State;

            internal ChooseZeroEnumerator()
            {
                Reset();
            }

            public T[] Current
            {
                get
                {
                    if (_State == State.OnElement)
                    {
                        return new T[0];
                    }
                    else
                    {
                        throw new InvalidOperationException();
                    }
                }
            }

            public void Dispose()
            {
                // Do nothing.
            }

            object IEnumerator.Current
            {
                get
                {
                    return Current;
                }
            }

            public bool MoveNext()
            {
                switch (_State)
                {
                    case State.BeforeBeginning:
                        _State = State.OnElement;
                        return true;

                    case State.OnElement:
                    case State.PastEnd:
                        _State = State.PastEnd;
                        return false;

                    default:
                        throw new InvalidOperationException();
                }
            }

            public void Reset()
            {
                _State = State.BeforeBeginning;
            }
        }
    }
}
