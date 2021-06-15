using System;
using System.IO;
using CppSharp;
using CppSharp.AST;
using CppSharp.Passes;

namespace dotnet_wrapper_generator
{
    class AtsFooters : ILibrary
    {
        public override void Setup(Driver driver)
        {
            var options = driver.Options;
            var module = options.AddModule("atsfooters");
            module.Headers.Add("atsfooters.hpp");
            options.OutputDir = "../dotnet-wrapper";
            options.GenerateClassTemplates = true;

            var parserOptions = driver.ParserOptions;
            parserOptions.AddIncludeDirs(GetIncludeDirectory());
        }

        public override void SetupPasses(Driver driver)
        {

        }

        public override void Preprocess(Driver driver, ASTContext context)
        {

        }

        public override void Postprocess(Driver driver, ASTContext context)
        {

        }

        public static string GetIncludeDirectory()
        {
            var directory = Directory.GetParent(Directory.GetCurrentDirectory());

            while (directory != null)
            {
                var path = Path.Combine(directory.FullName, "atsfooters","include");

                if (Directory.Exists(path))
                    return path;

                directory = directory.Parent;
            }

            throw new Exception(string.Format(
                "Include directory for project was not found"));
        }
    }

    static class Program
    {
        public static void Main(string[] args)
        {
            ConsoleDriver.Run(new AtsFooters());
        }
    }
}
