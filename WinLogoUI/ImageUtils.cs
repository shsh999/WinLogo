using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows.Media.Imaging;

namespace WinLogoUI
{
    class ImageUtils
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr LoadLibraryEx(string lib, IntPtr hFile, uint flags);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern void FreeLibrary(IntPtr module);
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr GetProcAddress(IntPtr module, string proc);

        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr LoadBitmap(IntPtr hInstance, int resourceID);

        [DllImport("gdi32.dll", SetLastError = true)]
        static extern bool DeleteObject(IntPtr hObject);


        const uint LOAD_LIBRARY_AS_DATAFILE = 0x00000002;
        const uint DONT_RESOLVE_DLL_REFERENCES = 0x00000001;

        private static string MakeTempWinlogoPath()
        {
            return $"{Path.GetTempFileName()}_winlogo_temp.dll";
        }

        private static Bitmap GetBitmapResource(IntPtr dll, int resource)
        {
            var bitmap = IntPtr.Zero;
            try
            {
                bitmap = LoadBitmap(dll, resource);
                return Bitmap.FromHbitmap(bitmap);
            }
            finally
            {
                if (bitmap != IntPtr.Zero)
                {
                    DeleteObject(bitmap);
                }
            }
        }

        private static BitmapImage LoadImage(Image bitmap)
        {
            using (MemoryStream memory = new MemoryStream())
            {
                bitmap.Save(memory, ImageFormat.Bmp);
                memory.Position = 0;
                BitmapImage bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = memory;
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapImage.EndInit();
                return bitmapImage;
            }
        }

        public static Dictionary<string, BitmapImage> LoadImagesFromLogoDll(bool fromResource)
        {
            string path = fromResource ? MakeTempWinlogoPath() : WinLogo.WINLOGO_PATH;
            byte[] winlogoData = WinLogo.GetWinLogoData();
            IntPtr dll = IntPtr.Zero;
            var result = new Dictionary<string, BitmapImage>();
            try
            {
                if (fromResource)
                {
                    using (FileStream fs = new FileStream(path, FileMode.CreateNew))
                    {
                        fs.Write(winlogoData, 0, winlogoData.Count());
                        fs.Flush();
                    }
                }

                dll = LoadLibraryEx(path, IntPtr.Zero, LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES);
                if (dll != IntPtr.Zero)
                {
                    result["enabled"] = LoadImage(GetBitmapResource(dll, 101));
                    result["disabled"] = LoadImage(GetBitmapResource(dll, 102));
                }
            }
            finally
            {
                if (dll != IntPtr.Zero)
                {
                    FreeLibrary(dll);
                }
                if (fromResource)
                {
                    File.Delete(path);
                }
            }

            return result;
        }
    }
}
