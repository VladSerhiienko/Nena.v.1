using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Windows.Forms;

namespace SmallTalk.Cipher
{
    class DES
    {
        public static void Encrypt(String input, String key, String initvec, String path)
        {
            byte[] txtbytearr = UnicodeEncoding.UTF8.GetBytes(input);

            using (DESCryptoServiceProvider descryptoservice = new DESCryptoServiceProvider())
            {
                try
                {
                    descryptoservice.Key = Encoding.ASCII.GetBytes(key);
                    descryptoservice.IV = Encoding.ASCII.GetBytes(initvec);

                    FileStream fstream = new FileStream(path, FileMode.Create, FileAccess.Write);
                    CryptoStream cryptostream = new CryptoStream(fstream, descryptoservice.CreateEncryptor(), CryptoStreamMode.Write);
                    cryptostream.Write(txtbytearr, 0, txtbytearr.Length);
                    cryptostream.Close();

                    fstream.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                }
            }
        }

        public static String Decrypt(String key, String initvec, String path)
        {
            string result = "";

            using (DESCryptoServiceProvider descryptoservice = new DESCryptoServiceProvider())
            {
                try
                {
                    descryptoservice.Key = Encoding.ASCII.GetBytes(key);
                    descryptoservice.IV = Encoding.ASCII.GetBytes(initvec);

                    FileStream fstream = new FileStream(path, FileMode.Open, FileAccess.Read);
                    CryptoStream cryptostream = new CryptoStream(fstream, descryptoservice.CreateDecryptor(), CryptoStreamMode.Read);
                    StreamReader reader = new StreamReader(cryptostream);
                    result = reader.ReadToEnd();

                    reader.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                }
            }

            return result;
        }
    }
}
