using Microsoft.Win32;
using RestartManager;
using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Windows;

namespace WinLogoUI
{
    class WinLogo
    {
        public static readonly string WINLOGO_PATH = Path.Combine(Environment.SystemDirectory, "Winlogo.dll");
        private static readonly string WINLOGO_KEY = @"Software\Microsoft\Windows\CurrentVersion\Explorer\ShellIconOverlayIdentifiers\   WinLogo";

        private static void runRegsvr(bool install)
        {
            string installArg = install ? "" : "/u";
            var process = new System.Diagnostics.Process
            {

                StartInfo = new ProcessStartInfo
                {
                    FileName = Path.Combine(Environment.SystemDirectory, "regsvr32.exe"),
                    Arguments = $"/s {installArg} {WINLOGO_PATH}",
                    CreateNoWindow = true
                }
            };
            process.Start();
            process.WaitForExit();
        }

        private static void TryDeleteWinlogo()
        {
            using (var rm = new RestartManagerSession())
            {
                rm.RegisterProcess(Process.GetProcessesByName("explorer"));
                rm.Shutdown(RestartManagerSession.ShutdownType.Normal);
                rm.Restart();
            }

            File.Delete(WINLOGO_PATH);
        }

        /// <summary>
        /// Checks whether WinLogo is installed by checking its registration as an Icon Overlay.
        /// </summary>
        public static bool IsInstalled()
        {
            using (var hklm = RegistryKey.OpenBaseKey(RegistryHive.LocalMachine, RegistryView.Registry64))
            using (var key = hklm.OpenSubKey(WINLOGO_KEY))
            {
                return key != null;
            }
        }

        public static byte[] GetWinLogoData()
        {
            using (Stream stream = Assembly.GetExecutingAssembly().GetManifestResourceStream("WinLogoDLL"))
            {
                if (stream == null)
                    return null;

                byte[] rawBytes = new byte[stream.Length];
                stream.Read(rawBytes, 0, rawBytes.Length);
                return rawBytes;
            }
        }

        /// <summary>
        /// Install Winlogo by dumping the dll to the disk and registering it with regsvr32.
        /// </summary>
        public static void Install()
        {
            try
            {
                byte[] winlogoData = GetWinLogoData();
                File.WriteAllBytes(WINLOGO_PATH, winlogoData);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error creating Winlogo.dll", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            try
            {
                runRegsvr(true);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error registering Winlogo.dll", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        public static void Uninstall()
        {
            try
            {
                runRegsvr(false);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error unregistering Winlogo", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            TryDeleteWinlogo();
        }
    }
}
