﻿using System;
using System.Collections.Generic;
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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EntityEditor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public static MainWindow Instance;

        public MainWindow()
        {
            Instance = this;
            InitializeComponent();
        }

        private void ButtonPrefab(object sender, RoutedEventArgs e)
        {
            new PrefabEditor().Show();
        }
    }
}