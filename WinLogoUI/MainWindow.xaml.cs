using System;
using System.IO;
using System.Windows;

using System.Diagnostics;
using System.Reflection;
using System.Globalization;
using Microsoft.Win32;
using System.Windows.Media.Imaging;
using System.Collections.Generic;

namespace WinLogoUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool isUserAction = true;

        private string enabledPath = null;
        private string disabledPath = null;

        private Dictionary<int, BitmapImage> resourceImages;

        private void SetSwitchNoAction(bool isChecked)
        {
            isUserAction = false;
            Switch.IsChecked = isChecked;
            isUserAction = true;
        }

        private static Assembly OnResolveAssembly(object sender, ResolveEventArgs args)
        {
            Assembly executingAssembly = Assembly.GetExecutingAssembly();
            AssemblyName assemblyName = new AssemblyName(args.Name);
 
            string path = assemblyName.Name + ".dll";
            if (assemblyName.CultureInfo.Equals(CultureInfo.InvariantCulture) == false)
            {
                path = String.Format(@"{0}\{1}", assemblyName.CultureInfo, path);
            }
 
            using (Stream stream = executingAssembly.GetManifestResourceStream(path))
            {
                if (stream == null)
                    return null;

                byte[] assemblyRawBytes = new byte[stream.Length];
                stream.Read(assemblyRawBytes, 0, assemblyRawBytes.Length);
                return Assembly.Load(assemblyRawBytes);
            }
        }

        public MainWindow()
        {
            // Register embedded DLL resolving
            AppDomain.CurrentDomain.AssemblyResolve += OnResolveAssembly;
            InitializeComponent();
            try
            {
                var installed = WinLogo.IsInstalled();
                SetSwitchNoAction(installed);
                resourceImages = ImageUtils.LoadImagesFromLogoDll(!installed);
                DisabledImage.Source = resourceImages[WinLogo.DISABLED_RESOURCE];
                EnabledImage.Source = resourceImages[WinLogo.ENABLED_RESOURCE];
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message, "Error initializing installer", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void Switch_Checked(object sender, RoutedEventArgs e)
        {
            if (isUserAction)
            {
                if (!WinLogo.IsInstalled())
                {
                    WinLogo.Install(enabledPath, disabledPath);
                }
            }
        }

        private void Switch_Unchecked(object sender, RoutedEventArgs e)
        {
            if (isUserAction)
            {
                if (WinLogo.IsInstalled())
                {
                    WinLogo.Uninstall();
                }
            }
        }

        private void DumpCustomizedWinlogo(string path)
        {
            WinLogo.DumpCustomizedWinlogo(path, enabledPath, disabledPath);
        }

        private void ExportDll(object sender, RoutedEventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.FileName = "Winlogo.dll";
            dialog.Filter = "Dynamically Linked Libraries (*.dll)|*.dll";
            if (dialog.ShowDialog() == true)
            {
                DumpCustomizedWinlogo(dialog.FileName);
            }
        }

        private void SaveInstaller(string path)
        {
            WinLogo.DumpCustomizedInstaller(path, enabledPath, disabledPath);
        }

        private void ExportInstaller(object sender, RoutedEventArgs e)
        {
            SaveFileDialog dialog = new SaveFileDialog();
            dialog.FileName = "WinLogoUI.exe";
            dialog.Filter = "Executable Files (*.exe)|*.exe";
            if (dialog.ShowDialog() == true)
            {
                SaveInstaller(dialog.FileName);
            }
        }

        private static void LoadImageFile(ref string pathVariable, System.Windows.Controls.Image imageToChange, System.Windows.Controls.Button revertButton)
        {
            OpenFileDialog dialog = new OpenFileDialog();
            dialog.Filter = "Image Files|*.bmp;*.png;*.jpg;*.jpeg";
            if (dialog.ShowDialog() == true)
            {
                pathVariable = dialog.FileName;
                imageToChange.Source = ImageUtils.LoadImageFromFile(pathVariable, true);
                revertButton.Visibility = Visibility.Visible;
            }
        }

        private void DisabledImageMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            LoadImageFile(ref disabledPath, DisabledImage, RevertDisabled);
        }

        private void EnabledImageMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            LoadImageFile(ref enabledPath, EnabledImage, RevertEnabled);
        }

        private void RevertEnabled_Click(object sender, RoutedEventArgs e)
        {
            EnabledImage.Source = resourceImages[WinLogo.ENABLED_RESOURCE];
            enabledPath = null;
            RevertEnabled.Visibility = Visibility.Hidden;
        }

        private void RevertDisabled_Click(object sender, RoutedEventArgs e)
        {
            DisabledImage.Source = resourceImages[WinLogo.DISABLED_RESOURCE];
            disabledPath = null;
            RevertDisabled.Visibility = Visibility.Hidden;
        }
    }
}
