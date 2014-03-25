using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SmallTalk.Labwork.Analysis.Utility
{
    /// <summary>
    /// Stores a set of elements from which combinations may be drawn.
    /// </summary>
    /// <typeparam name="T">Element type</typeparam>
    public class ElementSet<T>
    {
        private SortedList<T, int> _Elements;
        private int _Count;
        private int[] _ElementsAtOrAfter;

        /// <summary>
        /// Constructs a new <c>ElementSet</c> from a collection of elements, using the default comparer for the element type.
        /// </summary>
        /// <param name="elements">the elements</param>
        public ElementSet(IEnumerable<T> elements)
            : this(elements, null)
        {
        }

        /// <summary>
        /// Constructs a new <c>ElementSet</c> from a collection of an elements, using a custom comparer.
        /// </summary>
        /// <param name="elements">the elements</param>
        /// <param name="comparer">the comparer</param>
        public ElementSet(IEnumerable<T> elements, IComparer<T> comparer)
        {
            if (elements == null)
            {
                throw new ArgumentNullException("elements");
            }

            // Copy into SortedDictionary first, then into SortedList.
            // Inserting unsorted data in SortedList is O(n^2).
            // Inserting unsorted data in SortedDictionary is O(n log(n)).
            // Making a SortedList from sorted data is O(n).
            // Hence using a SortedDictionary in the middle is faster.
            SortedDictionary<T, int> sorted = new SortedDictionary<T, int>(comparer);
            foreach (T element in elements)
            {
                int count;
                if (sorted.TryGetValue(element, out count))
                {
                    sorted[element] = count + 1;
                }
                else
                {
                    sorted[element] = 1;
                }
                _Count++;
            }

            _Elements = new SortedList<T, int>(sorted, comparer);
            _Elements.Capacity = _Elements.Count;

            // Produce the Number of Elements At or After array.
            // This is a cumulative sum of the reverse of the values array.
            if (_Elements.Count > 0)
            {
                _ElementsAtOrAfter = new int[_Elements.Count];
                _ElementsAtOrAfter[_Elements.Count - 1] = _Elements.Values[_Elements.Count - 1];
                for (int i = _Elements.Count - 2; i >= 0; i--)
                {
                    _ElementsAtOrAfter[i] = _ElementsAtOrAfter[i + 1] + _Elements.Values[i];
                }
            }
        }

        internal SortedList<T, int> Elements
        {
            get
            {
                return _Elements;
            }
        }

        internal int Count
        {
            get
            {
                return _Count;
            }
        }

        internal int[] ElementsAtOrAfter
        {
            get
            {
                return _ElementsAtOrAfter;
            }
        }
    }
}
