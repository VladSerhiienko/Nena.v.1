using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Excel = Microsoft.Office.Interop.Excel;
using Office = Microsoft.Office.Core;
using Microsoft.Office.Tools.Excel;

namespace SmallTalk.Analysis
{
    using SmallTalk.FlowControl;

    class TextAnalyzer : ICommandListener
    {
        public ICommandListener AlphabetAnalyzer;
        public ICommandListener NGramAnalyzer;

        public TextAnalyzer()
        {
            AlphabetAnalyzer = new AlphabetAnalyzer();
            NGramAnalyzer = new NGramAnalyzer();
        }

        public void Handle(Ribbons.CommandPromptForm sender, ICommand cmd, int start, int end)
        {
            var paramCollection = new ICommand.IParameter[end - start];
            for (var i = start; i < end; i++) paramCollection[i] = cmd[i];

            if (paramCollection[0].Type.ToLowerInvariant() == "obj" &&
                paramCollection[0][0].ToLowerInvariant() == "analyzer")
            {
                if (paramCollection[0][1].ToLowerInvariant() == "alpha")
                {
                    AlphabetAnalyzer.Handle(sender, cmd, 1, end);
                }
                else if (paramCollection[0][1].ToLowerInvariant() == "ngram")
                {
                    NGramAnalyzer.Handle(sender, cmd, 1, end);
                }
            }
        }
    }
}

/*

$ /Set Src C:\Users\Vlad Serhiienko\Downloads\Papers\Variants\lab_1\C_C_VT00
$ /Set Dst AlphabeticAnalysis
$ /M:Analyzer:Alphabetical /Act:Analyze /Param:Srcpath:%Src /Param:Destpath:%Dst

*/