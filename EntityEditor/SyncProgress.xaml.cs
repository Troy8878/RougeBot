using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace EntityEditor
{
    /// <summary>
    /// Interaction logic for SyncProgress.xaml
    /// </summary>
    public partial class SyncProgress
    {
        private bool _close;

        public SyncProgress()
        {
            InitializeComponent();
        }

        public void SetMessage(string message)
        {
            Dispatcher.BeginInvoke(new Action(delegate
            {
                Status.Text = message;
            }));
        }

        public void SetProgress(double? progress)
        {
            Dispatcher.BeginInvoke(new Action(delegate
            {
                if (progress == null)
                {
                    Progress.IsIndeterminate = true;
                }
                else
                {
                    Progress.IsIndeterminate = false;
                    Progress.Value = (double) progress;
                }
            }));
        }

        public void CloseProgress()
        {
            _close = true;
            Close();
        }

        protected override void OnClosing(CancelEventArgs e)
        {
            if (!_close)
            {
                e.Cancel = true;
            }
        }
    }
}
