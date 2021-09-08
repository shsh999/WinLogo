using Microsoft.Win32;
using RestartManager;
using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Threading;
using System.Windows;
using System.Linq;
using System.Collections.Generic;
using System.ComponentModel;
using Mono.Cecil;

namespace WinLogoUI
{
    class WinLogo
    {
        public static readonly string WINLOGO_PATH = Path.Combine(Environment.SystemDirectory, "Winlogo.dll");
        private static readonly string WINLOGO_KEY = @"Software\Microsoft\Windows\CurrentVersion\Explorer\ShellIconOverlayIdentifiers\   WinLogo";

        public static readonly int DISABLED_RESOURCE = 101;
        public static readonly int ENABLED_RESOURCE = 102;

        private static void RunRegsvr(bool install)
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

        private static IEnumerable<Process> ListExplorerProcesses()
        {
            var currentSessionId = Process.GetCurrentProcess().SessionId;
            return Process.GetProcessesByName("explorer").Where(p => p.SessionId == currentSessionId);
        }

        private static void ResetExplorer()
        {
            try
            {
                // Try to reset explorer using the restart manager, as it is "prettier" and will result in reopening the same windows.
                // If it fails, fallback to the stupid "kill all" way.
                using (var rm = new RestartManagerSession())
                {
                    rm.RegisterProcess(Process.GetProcessesByName("explorer"));
                    rm.Shutdown(RestartManagerSession.ShutdownType.ForceShutdown);
                    rm.Restart();
                }
            }
            catch (Exception)
            {
                foreach (var process in ListExplorerProcesses())
                {
                    process.Kill();
                }
                if (!ListExplorerProcesses().Any())
                {
                    string explorer = string.Format("{0}\\{1}", Environment.GetEnvironmentVariable("WINDIR"), "explorer.exe");
                    new Process
                    {
                        StartInfo = new ProcessStartInfo
                        {
                            FileName = explorer
                        }
                    }.Start();
                }
            }
        }

        private static void TryDeleteWinlogoFile()
        {
            for (var i = 0; i < 5; ++i)
            {
                try
                {
                    File.Delete(WINLOGO_PATH);
                    return;
                }
                catch (Exception)
                {
                    if (i == 4)
                    {
                        throw;
                    }
                    Thread.Sleep(1000);
                }
            }
        }

        private static void TryDeleteWinlogo()
        {
            ResetExplorer();
            TryDeleteWinlogoFile();
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
                {
                    return null;
                }

                byte[] rawBytes = new byte[stream.Length];
                stream.Read(rawBytes, 0, rawBytes.Length);
                return rawBytes;
            }
        }

        public static void DumpCustomizedWinlogo(string path, string enabledPath, string disabledPath)
        {
            File.WriteAllBytes(path, GetWinLogoData());
            if (enabledPath != null || disabledPath != null)
            {
                using (var session = new ResourceUpdateSession(path))
                {
                    if (enabledPath != null)
                    {
                        session.UpdateResource(WinLogo.ENABLED_RESOURCE, ImageUtils.MakeResourceBitmapFromFile(enabledPath, true));
                    }
                    if (disabledPath != null)
                    {
                        session.UpdateResource(WinLogo.DISABLED_RESOURCE, ImageUtils.MakeResourceBitmapFromFile(disabledPath, true));
                    }
                    session.EndUpdate();
                }
            }
        }

        private static byte[] GetCustomizedWinlogo(string enabledPath, string disabledPath)
        {
            using (var tempFile = new TempFile())
            {
                DumpCustomizedWinlogo(tempFile.Path, enabledPath, disabledPath);
                return File.ReadAllBytes(tempFile.Path);
            }
        }

        public static void DumpCustomizedInstaller(string path, string enabledPath, string disabledPath)
        {
            // If this is the same as the embedded images, just copy the executable
            if (enabledPath == null && disabledPath == null)
            {
                File.Copy(Assembly.GetExecutingAssembly().Location, path, true);
                return;
            }

            var assemblyDefinition = AssemblyDefinition.ReadAssembly(Assembly.GetExecutingAssembly().Location);
            
            // Remove the old dll resource
            var oldResource = assemblyDefinition.MainModule.Resources.FirstOrDefault(r => r.Name == "WinLogoDLL");
            if (oldResource != null)
            {
                assemblyDefinition.MainModule.Resources.Remove(oldResource);
            }

            // Add the new dll resource
            byte[] dllData = GetCustomizedWinlogo(enabledPath, disabledPath);
            var newResource = new EmbeddedResource("WinLogoDLL", ManifestResourceAttributes.Public, dllData);
            assemblyDefinition.MainModule.Resources.Add(newResource);

            // Write to the requested path
            assemblyDefinition.Write(path);
        }

        /// <summary>
        /// Install Winlogo by dumping the dll to the disk and registering it with regsvr32.
        /// </summary>
        public static void Install(string enabledPath, string disabledPath)
        {
            try
            {
                DumpCustomizedWinlogo(WINLOGO_PATH, enabledPath, disabledPath);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error creating Winlogo.dll", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            try
            {
                RunRegsvr(true);
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
                RunRegsvr(false);
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error unregistering Winlogo", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }

            try
            {
                TryDeleteWinlogo();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error deleting Winlogo", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
        }
    }
}
