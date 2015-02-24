using System.Windows;
using System.Windows.Input;

namespace EntityEditor
{
    /// <summary>
    ///     Interaction logic for PrefabEditor.xaml
    /// </summary>
    public partial class PrefabEditor
    {
        public PrefabEditor()
        {
            InitializeComponent();
        }

        private void OnOpen(object sender, RoutedEventArgs e)
        {
            EditHistory.ClearHistory();

            Room.Load();
            e.Handled = true;
        }

        private void OnSave(object sender, RoutedEventArgs e)
        {
            Room.Save();
            e.Handled = true;
        }

        private void OnUndo(object sender, RoutedEventArgs e)
        {
            EditHistory.Undo();
            e.Handled = true;
        }

        private void OnRedo(object sender, RoutedEventArgs e)
        {
            EditHistory.Redo();
            e.Handled = true;
        }

        private void AlwaysExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        private void OnNew(object sender, ExecutedRoutedEventArgs e)
        {
            Room.SetEmpty();
        }
    }
}