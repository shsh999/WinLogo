using System;
using System.IO;

namespace WinLogoUI
{
    /// <summary>
    /// This class is used to generate temorary file paths and delete them on dispose.
    /// </summary>
    class TempFile : IDisposable
    {
        public TempFile()
        {
            Path = $"{System.IO.Path.GetTempFileName()}_winlogo_temp.dll";
        }

        public string Path { get; private set; }

        ~TempFile()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (Path != null)
            {
                if (File.Exists(Path))
                {
                    File.Delete(Path);
                }

                Path = null;
            }
        }
    }
}
