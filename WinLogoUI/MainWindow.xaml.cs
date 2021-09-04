using System;
using System.IO;
using System.Windows;

using System.Diagnostics;
using System.Reflection;
using System.Globalization;

namespace WinLogoUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private bool isUserAction = true;

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
            foreach (var x in Assembly.GetExecutingAssembly().GetManifestResourceNames())
            {
                Debug.WriteLine(x);
            }
            InitializeComponent();
            try
            {
                var installed = WinLogo.IsInstalled();
                SetSwitchNoAction(installed);
                var images = ImageUtils.LoadImagesFromLogoDll(!installed);
                FirstImage.Source = images["enabled"];
                SecondImage.Source = images["disabled"];
            }
            catch (Exception e)
            {
                Debug.WriteLine(e);
            }
        }

        private void Switch_Checked(object sender, RoutedEventArgs e)
        {
            if (isUserAction)
            {
                if (!WinLogo.IsInstalled())
                {
                    WinLogo.Install();
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
    }
}
