using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Common;

namespace LempelZivWelch
{
    class LZWCodec
    {
        public LZWDictionary Dictionary 
        { 
            get; 
            private set; 
        }

        public LZWCodec(string message)
        {
            Dictionary = new LZWDictionary();
            Dictionary.Initialize(message);
        }

        public LZWCodec(LZWDictionary dictionary)
        {
            Dictionary = dictionary;
        }

        public uint[] Encode(string decoded)
        {
            Supervisor.Write("Encoding: " + decoded);

            if (!ValidateDecodedMessage(decoded))
            {
                Supervisor.Write("Error: Decoded message is not valid.");
                return null;
            }

            List<uint> encoded = new List<uint>();

            int lengthMax = decoded.Length;
            int indexCurrent = 0;
            int lengthCurrent = 1;

            while (indexCurrent < lengthMax)
            {
                string wordCurrent = decoded.Substring(indexCurrent, lengthCurrent);
                int indexWordCurrent = Dictionary.Add(wordCurrent);

                if (wordCurrent == Dictionary.First().Word)
                {
                    encoded.Add(Dictionary.First().Index);
                    Supervisor.Write("Added to encoding set: Word = {0} Index = {1}", Dictionary.First().Word, Dictionary.First().Index);
                    break;
                }

                if (LZWDictionary.IsAdded(indexWordCurrent))
                {
                    string previousWord = wordCurrent.Substring(0, wordCurrent.Length - 1);
                    int previousWordIndex = Dictionary.Get(previousWord);
                    encoded.Add((uint)previousWordIndex);

                    Supervisor.Write("Added to encoding set: Word = {0} Index = {1}", previousWord, previousWordIndex);

                    indexCurrent += lengthCurrent - 1;
                    lengthCurrent = 1;
                }
                else
                {

                    lengthCurrent++;
                }
            }

            return encoded.ToArray();
        }

        public string Decode(uint[] encoded)
        {
            if (!ValidateEncodedMessage(encoded))
            {
                Supervisor.Write("Error: Encoded message is not valid.");
                return null;
            }

            string decoded = string.Empty;
            int lengthMax = encoded.Length;
            int indexCurrent = 0;
            int lengthCurrent = 1;

            while (indexCurrent < lengthMax)
            {
                uint[] currentEncodedWord = Take(encoded, indexCurrent, lengthCurrent);
                string currentDecodedWord = Dictionary.Get(currentEncodedWord.First());

                if (lengthCurrent == 2)
                {
                    currentDecodedWord += Dictionary.Get(currentEncodedWord.Last())[0].ToString();
                }

                int indexCurrentWord = Dictionary.Add(currentDecodedWord);

                if (currentEncodedWord[0].Equals(0u))
                {
                    decoded += Dictionary.First().Word;
                    Supervisor.Write("Added to decoded set: Word = {0} Index = {1}", Dictionary.First().Word, Dictionary.First().Index);
                    break;
                }

                if (LZWDictionary.IsAdded(indexCurrentWord))
                {
                    string previousWord = Dictionary.Get(currentEncodedWord.First()); // + Dictionary.Get(currentEncodedWord.Last())[0].ToString();
                    int previousWordIndex = Dictionary.Get(previousWord);

                    decoded += previousWord;
                    Supervisor.Write("Added to encoding set: Word = {0} Index = {1}", previousWord, previousWordIndex);

                    indexCurrent += lengthCurrent - 1;
                    lengthCurrent = 1;
                }
                else
                {

                    lengthCurrent++;
                }
            }

            return decoded;
        }

        public static TData[] Take<TData>(TData[] data, int startIndex, int length)
        {
            TData[] result = new TData[length];

            for (int i = startIndex, j = 0; i < startIndex + length; i++, j++)
            {
                result[j] = data[i];
            }

            return result;
        }

        public static string ToString<TData>(TData[] data)
        {
            string result = string.Empty;

            foreach (var item in data)
            {
                result += item.ToString() + " ";
            }

            return result;
        }

        public bool ValidateDecodedMessage(string decoded)
        {
            if (decoded.Last() != '#')
            {
                return false;
            }

            return true;
        }

        public bool ValidateEncodedMessage(uint[] encoded)
        {
            if (encoded.Last() != 0u)
            {
                return false;
            }

            return true;
        }



    }
}
