using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace SmallTalk.FlowControl
{
    /// <summary>
    /// Represents the command interface
    /// </summary>
    public abstract class ICommand
    {
        /// <summary>
        /// Represents the very basic command parameter properties
        /// </summary>
        public interface IParameter
        {
            /// <summary>
            /// Parameter type, e.g. Module, Action, ...
            /// </summary>
            string Type { get; }
            string this[int QualifierIndex] { get; }
            int Count { get; }
        }

        /// <summary>
        /// Parses command string into the parameter collection
        /// </summary>
        /// <param name="cmd">Command string</param>
        public abstract void Parse(string cmd);
        /// <summary>
        /// Accesses the parameter at provided position
        /// </summary>
        /// <param name="ParameterIndex">Parameter position</param>
        /// <returns>Parameter</returns>
        public abstract IParameter this[int ParameterIndex] { get; }
        public abstract int Count { get; }
    }

    /// <summary>
    /// Command implementation class
    /// </summary>
    internal class Command : ICommand
    {
        /// <summary>
        /// Represents the command parameter implementation
        /// </summary>
        protected class Parameter : ICommand.IParameter
        {
            public String ParameterType;
            public IList<string> Qualifiers;

            public string Type { get { return ParameterType; } }
            public string this[int Index] { get { return Qualifiers[Index]; } }

            public Parameter(string type = null, List<string> values = null)
            {
                Qualifiers = values != null ? values : (new List<string>());
                ParameterType = type != null ? type : String.Empty;
            }

            public override string ToString()
            {
                string result = string.Empty;
                result += "[T= " + ParameterType + ": Q= (";
                for (var i = 0; i < Qualifiers.Count - 1; i++)
                    result += Qualifiers[i] + ", ";
                result += Qualifiers.Last() + ")]";
                return result;
            }


            public int Count
            {
                get { return Qualifiers.Count; }
            }
        };

        // Command as string
        protected String Ctx;
        protected IList<ICommand.IParameter> Params;

        public Command()
        {
            // Zero the string value first
            Ctx = System.String.Empty;
            // Then initialize cmd params 
            Params = new List<ICommand.IParameter>();
        }

        public override void Parse(System.String Cmd)
        {
            if (Cmd == null || Cmd.Length == 0)
            {
                throw new System.ArgumentNullException(
                    "Nothing to parse (null or  empty command)."
                    );
            }

            // Remove all the leading and trailing whitespaces from the cmd
            Ctx = Cmd = Cmd.Trim();

            // Check if its command
            if (Cmd.Substring(0,2) != "$ ")
            {
                throw new System.ArgumentException(
                    "The provided string is not a command " + 
                    "('$' at the start is missing)."
                    );
            }
            else
            {
                Cmd = Cmd.Substring(2).Trim();

                // Its a command, so in case if object reusage clear the 
                // used params. 
                Params.Clear();
            }

            // Split the command by spaces
            char[] Space = { ' ' };
            var paramCollection = Cmd.Split(Space, StringSplitOptions.RemoveEmptyEntries);

            // Loop through all the params in the command list to fill out 
            // the member parameter collection 
            foreach (var param in paramCollection)
            {
                char[] Dots = { ':' };
                var valueCollection = param.Split(Dots, StringSplitOptions.RemoveEmptyEntries);

                if (valueCollection.Length == 0)
                {
                    throw new System.ArgumentException(
                        "Invalid parameter formatting at \"" + param + "\". " + 
                        " At least one parameter qualifier should be provided."
                        );
                }

                // Process the first qualifier as it has some formatting issues to handle out 
                var firstQualifier = valueCollection.FirstOrDefault<String>();

                // Remove all the leading unused signs (some standard like '-' or '/')
                while (firstQualifier != null && firstQualifier.Length != 0 && 
                    firstQualifier.First<Char>() == '-' || 
                    firstQualifier.First<Char>() == '/')
                {
                    firstQualifier = firstQualifier.Substring(1);
                }

                var lastParam = new Parameter(values: new List<String>());
                lastParam.ParameterType = firstQualifier;

                for (var i = 1; i < valueCollection.Length; i++)
                    lastParam.Qualifiers.Add(valueCollection[i]);

                Params.Add(lastParam);
            }
        } // m:Parse

        public override string ToString()
        {
            string result = string.Empty;
            result += "[P= ";
            for (var i = 0; i < Params.Count - 1; i++)
                result += Params[i] + ", ";
            result += Params.Last() + "]";
            return result;
        }

        public override ICommand.IParameter this[int ParameterIndex]
        {
            get { return Params[ParameterIndex]; }
        }

        public override int Count
        {
            get { return Params.Count; }
        }
    } // c:Command
}
