using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using Vector = EntityEditor.Entities.Representations.Properties.Vector;

namespace EntityEditor.Entities.Representations.Views
{
    public class VectorView : StackPanel
    {
        public static readonly DependencyProperty FieldProperty = DependencyProperty.Register(
            "Field", typeof (Vector), typeof (VectorView), new PropertyMetadata(default(Vector)));

        static VectorView()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof (VectorView),
                new FrameworkPropertyMetadata(typeof (VectorView)));
        }

        public VectorView()
        {
            Orientation = Orientation.Horizontal;
        }

        public Vector Field
        {
            get { return (Vector) GetValue(FieldProperty); }
            set { SetValue(FieldProperty, value); }
        }

        public int Dimensions { get; set; }

        protected override void OnInitialized(EventArgs e)
        {
            if (Field.Dimensions > 0)
                AddField("X");
            if (Field.Dimensions > 1)
                AddField("Y");
            if (Field.Dimensions > 2)
                AddField("Z");
            if (Field.Dimensions > 3)
                AddField("W");

            base.OnInitialized(e);
        }

        private void AddField(string field)
        {
            Children.Add(new TextBlock
            {
                Text = field,
                Margin = new Thickness(2)
            });

            var box = new TextBox
            {
                Margin = new Thickness(2),
                Width = 50
            };
            var binding = new Binding(field) {Source = Field};
            BindingOperations.SetBinding(box, TextBox.TextProperty, binding);
            Children.Add(box);
        }
    }
}