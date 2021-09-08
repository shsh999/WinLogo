using System;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;

namespace WinLogoUI
{
    /// <summary>
    /// This class is used to encapsulate the modification of a native PE file's resources.
    /// Currently, it supports only the modification of bitmap resources.
    /// 
    /// The expected class usages is as follows:
    ///     using (var session = new ResourceUpdateSession(path))
    ///     {
    ///         session.UpdateResource(id1, bmp1);
    ///         session.UpdateResource(id2, bmp2);
    ///         ...
    ///         session.EndUpdate();
    ///     }
    /// </summary>
    class ResourceUpdateSession : IDisposable
    {
        [DllImport("kernel32.dll", SetLastError = true)]
        static extern IntPtr BeginUpdateResource(string pFileName, bool bDeleteExistingResources);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool EndUpdateResource(IntPtr hUpdate, bool fDiscard);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern bool UpdateResource(IntPtr hUpdate, IntPtr lpType, IntPtr lpName, ushort wLanguage, byte[] lpData, uint cbData);

        /// <summary>
        /// The native HANDLE receivied by BeginUpdateResource.
        /// </summary>
        private IntPtr resourceHandle;

        private static readonly IntPtr RT_BITMAP = new IntPtr(2);

        /// <summary>
        /// The size of a BITMAPFILEHEADER structure.
        /// </summary>
        private static readonly int BITMAP_FILE_HEADER_SIZE = 14;

        /// <summary>
        /// Initialize a new resource update session for the given dll path.
        /// </summary>
        /// <param name="dllPath">The path of the dll to be modified</param>
        /// <param name="removeAllResources">Whether to remove all existing resources</param>
        public ResourceUpdateSession(string dllPath, bool removeAllResources = false)
        {
            resourceHandle = BeginUpdateResource(dllPath, removeAllResources);
            if (resourceHandle == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        /// <summary>
        /// Update the bitmap resource with the given id to contain the input image.
        /// </summary>
        /// <param name="resourceId">The id of the resource to modify</param>
        /// <param name="bitmap">The new resource bitmap</param>
        public void UpdateResource(int resourceId, Bitmap bitmap)
        {
            using (MemoryStream stream = new MemoryStream())
            {
                // The format of a bitmap resource is a bmp file without the BITMAPFILEHEADER
                bitmap.Save(stream, ImageFormat.Bmp);
                byte[] imageData = new byte[stream.Length - BITMAP_FILE_HEADER_SIZE];
                // Start reading after the header
                stream.Position = BITMAP_FILE_HEADER_SIZE;
                stream.Read(imageData, 0, imageData.Length);

                if (!UpdateResource(resourceHandle, RT_BITMAP, new IntPtr(resourceId), 1037, imageData, (uint)imageData.Length))
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }

        /// <summary>
        /// End the resource update session.
        /// </summary>
        /// <param name="saveChanges">Whether to save or discard the changes in the original file</param>
        public void EndUpdate(bool saveChanges = true)
        {
            if (resourceHandle != IntPtr.Zero)
            {
                if (!EndUpdateResource(resourceHandle, !saveChanges))
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                resourceHandle = IntPtr.Zero;
            }
        }

        ~ResourceUpdateSession()
        {
            try
            {
                EndUpdate(false);
            }
            catch (Exception)
            {
                // Can't do anything...
            }
        }

        public void Dispose()
        {
            try
            {
                EndUpdate(false);
            }
            catch (Exception)
            {
                // Can't do anything...
            }
            GC.SuppressFinalize(this);
        }
    }
}
