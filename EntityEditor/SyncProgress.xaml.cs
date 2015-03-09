﻿using System;
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
using System.Windows.Threading;

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
            }), DispatcherPriority.Send);
        }

        public void SetProgress(double? progress)
        {
            Dispatcher.BeginInvoke(new Action(delegate
            {
                if (progress == null || progress > 1)
                {
                    Progress.IsIndeterminate = true;
                }
                else
                {
                    Progress.IsIndeterminate = false;
                    Progress.Value = (double) progress;
                }
            }), DispatcherPriority.Send);
        }

        public void CloseProgress()
        {
            if (_close)
                return;
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
