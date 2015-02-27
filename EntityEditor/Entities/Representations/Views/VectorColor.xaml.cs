using System.Windows;
using System.Windows.Media;
using Vector = EntityEditor.Entities.Representations.Properties.Vector;

namespace EntityEditor.Entities.Representations.Views
{
    /// <summary>
    ///     Interaction logic for VectorColor.xaml
    /// </summary>
    public partial class VectorColor
    {
        public VectorColor()
        {
            InitializeComponent();
        }

        public Vector Field
        {
            get { return (Vector) GetValue(FieldProperty); }
            set { SetValue(FieldProperty, value); }
        }

        private static void PropertyChangedCallback(DependencyObject dop, DependencyPropertyChangedEventArgs e)
        {
            var vector = (VectorColor) dop;
            var picker = vector.Picker;
            var field = (Vector) e.NewValue;

            picker.SelectedColor = new Color
            {
                ScR = (float) field.X,
                ScG = (float) field.Y,
                ScB = (float) field.Z,
                ScA = (float) field.W
            };
        }

        private void OnColor(object sender, RoutedPropertyChangedEventArgs<Color> e)
        {
            Field.X = e.NewValue.ScR;
            Field.Y = e.NewValue.ScG;
            Field.Z = e.NewValue.ScB;
            Field.W = e.NewValue.ScA;
        }

        public static readonly DependencyProperty FieldProperty = DependencyProperty.Register(
            "Field", typeof (Vector), typeof (VectorColor),
            new PropertyMetadata(default(Vector), PropertyChangedCallback));
    }
}