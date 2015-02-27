using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interactivity;
using System.Windows.Media;

namespace EntityEditor.Helpers
{
    public class ScrollTrigger : TriggerBase<FrameworkElement>
    {
        protected override void OnAttached()
        {
            AssociatedObject.Loaded += AssociatedObject_Loaded;
        }

        private void AssociatedObject_Loaded(object sender, RoutedEventArgs e)
        {
            foreach (var scrollViewer in GetScrollViewers())
                scrollViewer.ScrollChanged += scrollViewer_ScrollChanged;
        }

        private void scrollViewer_ScrollChanged(object sender, ScrollChangedEventArgs e)
        {
            InvokeActions(e.OriginalSource);
        }

        private IEnumerable<ScrollViewer> GetScrollViewers()
        {
            for (DependencyObject element = AssociatedObject;
                element != null;
                element = VisualTreeHelper.GetParent(element))
                if (element is ScrollViewer) yield return element as ScrollViewer;
        }
    }
}