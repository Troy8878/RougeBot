using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Windows;
using EntityEditor.Annotations;
using EntityEditor.API;

namespace EntityEditor
{
    /// <summary>
    ///     Interaction logic for AuthorInput.xaml
    /// </summary>
    public partial class AuthorInput : INotifyPropertyChanged
    {
        private Author _author;

        public AuthorInput()
        {
            Author = Author.Load();

            InitializeComponent();
        }

        public Author Author
        {
            get { return _author; }
            set
            {
                if (Equals(value, _author)) return;
                _author = value;
                OnPropertyChanged();
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        public event Action OnClose;

        private void DoneClick(object sender, RoutedEventArgs e)
        {
            Author.Pass = Password.Password;

            if (!Author.CheckCompleteness())
            {
                MessageBox.Show("Please fill in all of the fields");
                return;
            }

            Author.Save();
            if (OnClose != null)
                OnClose();
        }

        public void Reset()
        {
            Author = Author.Load();
        }

        [NotifyPropertyChangedInvocator]
        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            var handler = PropertyChanged;
            if (handler != null) handler(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}