using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Common;

namespace LempelZivWelch
{
    class LZWWord
    {
        private string _Word;
        public string Word
        {
            get { return _Word; }
            set { _Word = value; }
        }

        private uint _Index;
        public uint Index
        {
            get { return _Index; }
            set { _Index = value; }
        }

        public LZWWord()
            : this(string.Empty, uint.MinValue)
        {
        }

        public LZWWord(string word, uint index)
        {
            Supervisor.Write("Created: {0} {1} 0x{2:X}", word, index, (int)index);

            _Word = word;
            _Index = index;
        }

        public override string ToString()
        {
            return string.Format("{0} {1} 0x{2:X}", Word, Index, (int)Index);
        }
    }

    class LZWDictionary : List<LZWWord>
    {
        public bool Initialized
        {
            get;
            private set;
        }

        public uint IndexEndInitialDictionary 
        { 
            get; 
            private set; 
        }

        public int Size
        {
            get { return Count; }
        }

        public LZWDictionary()
        {
            Add("#");
            Initialized = false;
            IndexEndInitialDictionary = 0;
        }

        public void Initialize(string message)
        {
            if (Initialized)
            {
                Supervisor.Write("Warning: Already initialized.");
                Supervisor.Write("Initial dictionary:");
                WriteDictionary();
                return;
            }

            Supervisor.Write("Initializing with: " + message);

            foreach (var symbol in message)
            {
                Add(symbol.ToString());
            }

            IndexEndInitialDictionary = (uint)Count - 1;
            Initialized = true;
            Supervisor.Write("Index of the init dictionary: " + IndexEndInitialDictionary);
        }

        public int Add(string word)
        {
            if (!Exists(word))
            {
                Add(new LZWWord(word, (uint)Count));
                return Count - 1;
            }

            return -1;
        }

        public int Add(uint[] indices)
        {
            string word = Generate(indices);

            if (!Exists(word))
            {
                Add(new LZWWord(word, (uint)Count));
                return Count - 1;
            }

            return -1;
        }

        public bool Exists(string word)
        {
            foreach (var item in this)
            {
                if (item.Word.Equals(word))
                {
                    return true;
                }
            }

            return false;
        }

        public bool Exists(uint[] word)
        {
            foreach (var item in this)
            {
                if (item.Word.Equals(word))
                {
                    return true;
                }
            }

            return false;
        }

        public string Get(uint index)
        {
            foreach (var item in this)
            {
                if (item.Index.Equals(index))
                {
                    return item.Word;
                }
            }

            return null;
        }

        public int Get(string word)
        {

            foreach (var item in this)
            {
                if (item.Word.Equals(word))
                {
                    return (int)item.Index;
                }
            }

            return -1;
        }

        public string Generate(uint[] word)
        {
            string result = string.Empty;

            foreach (var item in word)
            {
                result += Get(item);
            }

            return result;
        }

        public void WriteDictionary(uint indexStart = 0u, uint indexEnd = 0u)
        {
            if (indexEnd == 0)
            {
                indexEnd = IndexEndInitialDictionary;
            }

            for (uint i = indexStart; i < indexEnd; i++)
            {
                Supervisor.Write(base[(int)i].ToString());
            }
        }

        public static bool IsAdded(int result)
        {
            return ((result != -1) ? true : false);
        }

    }
}
