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

        public static Bitmap ConvertTo24bpp(Image img)
        {
            var bmp = new Bitmap(img.Width, img.Height, PixelFormat.Format24bppRgb);
            using (var graphics = Graphics.FromImage(bmp))
            {
                graphics.DrawImage(img, new Rectangle(0, 0, img.Width, img.Height));
            }
            return bmp;
        }

        private static Size GetStretchedSize(Image image)
        {
            return new Size(32, 32 * image.Height / image.Width);
        }

        public static Bitmap MakeResourceBitmapFromFile(string path, bool stretch)
        {
            var image = Image.FromFile(path);
            var bitmap = stretch ? new Bitmap(image, GetStretchedSize(image)) : new Bitmap(image);
            bitmap = ConvertTo24bpp(bitmap);
            return bitmap;
        }

        public static BitmapImage LoadImageFromFile(string path, bool stretch)
        {
            return LoadImage(MakeResourceBitmapFromFile(path, stretch));
        }

        public static Dictionary<int, BitmapImage> LoadImagesFromLogoDll(bool fromResource)
        {
            string path = fromResource ? MakeTempWinlogoPath() : WinLogo.WINLOGO_PATH;
            byte[] winlogoData = WinLogo.GetWinLogoData();
            IntPtr dll = IntPtr.Zero;
            var result = new Dictionary<int, BitmapImage>();
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
                    result[WinLogo.DISABLED_RESOURCE] = LoadImage(GetBitmapResource(dll, WinLogo.DISABLED_RESOURCE));
                    result[WinLogo.ENABLED_RESOURCE] = LoadImage(GetBitmapResource(dll, WinLogo.ENABLED_RESOURCE));
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
