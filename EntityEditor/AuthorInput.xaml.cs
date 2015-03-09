using System;
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
using System.Windows.Shapes;
using EntityEditor.API;

namespace EntityEditor
{
    /// <summary>
    /// Interaction logic for AuthorInput.xaml
    /// </summary>
    public partial class AuthorInput : Window
    {
        public Author Author { get; set; }

        public AuthorInput()
        {
            Author = Author.Load();

            InitializeComponent();
        }

        private void DoneClick(object sender, RoutedEventArgs e)
        {
            Author.Pass = Password.Password;

            if (!Author.CheckCompleteness())
            {
                MessageBox.Show("Please fill in all of the fields");
                return;
            }

            Author.Save();
            Close();
        }
    }
}
