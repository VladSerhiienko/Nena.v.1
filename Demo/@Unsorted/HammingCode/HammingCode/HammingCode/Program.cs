using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;

namespace HammingCode
{
    class Program
    {
        static Random random = new Random();

        static void Main(string[] args)
        {
            Console.WriteLine("***  Sending  ***");
            Binary message;

            Console.WriteLine("Enter message in binary representation:");
            //string rawMessage = "1010";
            var arr = "10110110".Reverse().ToArray();
            string rawMessage = "10110110"; // new string(arr);
            //string rawMessage = "1110";
            message = new Binary(rawMessage.Select(c => c == '1' ? true : false));

            int columnsAmount = message.Count();
            int rowsAmount = (int) Math.Ceiling(Math.Log(columnsAmount, 2) + 1);
            
            BinaryMatrix H = GenerateHMatrix(rowsAmount, columnsAmount);
            Binary verification = GenerateVerificationBits(H, message);
            Binary frame = Binary.Concatenate(message, verification);

            Console.WriteLine("Message = {0}", message);
            Console.WriteLine("H matrix:");
            Console.Write(H);
            Console.WriteLine("Verification = {0}", verification);
            Console.WriteLine("Output Frame = {0}", frame);
            Console.WriteLine();


            Console.WriteLine("*** Receiving ***");
            Console.WriteLine("Corrupt message? [y/n]");
            bool? corruptMessage = null;
            do
            {
                switch (Console.ReadKey().Key)
                {
                    case ConsoleKey.Y:
                        corruptMessage = true;
                        break;
                    case ConsoleKey.N:
                        corruptMessage = false;
                        break;
                }       
                Console.WriteLine();
            } while (!corruptMessage.HasValue);
            
            Binary receivedFrame = new Binary(frame.ToArray());
            if (corruptMessage.Value)
            {
                int badBit = 7; //random.Next(0, receivedFrame.Length - 1);
                receivedFrame[badBit] = !receivedFrame[badBit];
            }
            Binary receivedMessage = new Binary(receivedFrame.Take(columnsAmount));     
            Binary receivedVerification = new Binary(receivedFrame.Skip(columnsAmount));
            
            H = GenerateHMatrix(rowsAmount, columnsAmount);
            Binary receivedMessageVerification = GenerateVerificationBits(H, receivedMessage);
            Binary s = receivedVerification ^ receivedMessageVerification;

            Console.WriteLine("received frame = {0}", receivedFrame);
            Console.WriteLine("H matrix:");
            Console.Write(H);
            Console.WriteLine("received message verification: {0}", receivedMessageVerification);
            Console.WriteLine("s value: {0}", s);
            
            if(s.CountOnes()>0)
            {
                try
                {
                    BinaryMatrix HWithIdentity = GenerateHWithIdentity(H);
                    int faultyBitPosition = FindFaultyBit(HWithIdentity, s);

                    Binary correctedFrame = new Binary(receivedFrame.ToArray());
                    correctedFrame[faultyBitPosition] = !correctedFrame[faultyBitPosition];

                    Console.WriteLine("H matrix with identity:");
                    Console.Write(HWithIdentity);
                    Console.WriteLine("Fault bit possition founded: {0}", faultyBitPosition);

                    Binary correctedFrameGeneratedVerify = GenerateVerificationBits(H, new Binary(correctedFrame.Take(columnsAmount)));
                    Binary correctedFrameVerify = new Binary(correctedFrame.Skip(columnsAmount));
                    Binary correctionVerify = correctedFrameVerify ^ correctedFrameGeneratedVerify;
                    if (correctionVerify.CountOnes() == 0)
                        Console.WriteLine("Corrected frame = {0}", correctedFrame);
                    else
                        Console.WriteLine("The frame cannot be corrected");
                }
                catch (WarningException)
                {
                    Console.WriteLine("The frame cannot be corrected");
                }
            }
            else
            {
                Console.WriteLine("The received frame is correct");
            }

            
            Console.ReadKey();
        }

        private static int FindFaultyBit(BinaryMatrix H, Binary s)
        {
            for (int i = 0; i < H.ColumnAmount; i++)
            {
                Binary column = H.GetColumn(i);
                Binary check = s ^ column;
                if(check.Any(b=>b))
                    continue;
                return i;
            }

            throw new WarningException("Faulty bit not found!");
        }

        static BinaryMatrix GenerateHWithIdentity(BinaryMatrix H)
        {
            BinaryMatrix HWithIdentity = new BinaryMatrix(H.RowAmount, H.ColumnAmount + H.RowAmount);
            for (int y = 0; y < H.RowAmount; y++)
            {
                for (int x = 0; x < H.ColumnAmount; x++)
                {
                    HWithIdentity.Set(y, x, H.Get(y, x));
                }
            }

            for (int y = 0; y < H.RowAmount; y++)
            {
                int n = 0;
                for (int x = H.ColumnAmount; x < H.ColumnAmount + H.RowAmount; x++)
                {
                    HWithIdentity.Set(y, x, y == n);

                    n++;
                }
            }
            return HWithIdentity;
        }
        
        static Binary GenerateVerificationBits(BinaryMatrix H, Binary message)
        {
            Binary verification = new Binary(new bool[H.RowAmount]);
            for (int i = 0; i < H.RowAmount; i++)
            {
                Binary row = H.GetRow(i);
                Binary addiction = row & message;
                bool verificationBit = addiction.CountOnes()%2 == 1 ? true : false;
                verification[i] = verificationBit;
            }
            return verification;
        }

        static BinaryMatrix GenerateHMatrix (int rowsAmount, int columnsAmount)
        {
            BinaryMatrix H = new BinaryMatrix(rowsAmount, columnsAmount);

            int n = 0;
            for (int i = 1; i <= Math.Pow(2, rowsAmount); i++)
            {
                Binary binary = new Binary(i, H.RowAmount);
                if (binary.CountOnes() >= 2)
                {
                    for (int y = 0; y < rowsAmount; y++)
                    {
                        H.Set(y, n, binary[y]);
                    }
                    n++;
                }
                if (n >= H.ColumnAmount)
                    break;
            }
            return H;
        }
    }
}
