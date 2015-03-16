using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using EntityEditor.Properties;
using MahApps.Metro;
using Newtonsoft.Json;

namespace EntityEditor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void App_OnDispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            MessageBox.Show(
                e.ToString() + "\n\nJSON view\n" + JsonConvert.SerializeObject(e),
                "FATAL ERROR", MessageBoxButton.OK, MessageBoxImage.Error);
        }

        protected override void OnStartup(StartupEventArgs e)
        {
            var theme = ThemeManager.DetectAppStyle(this);
            if (Settings.Default.DarkTheme)
            {
                ThemeManager.ChangeAppTheme(this, "BaseDark");
            }
        }
    }
}
