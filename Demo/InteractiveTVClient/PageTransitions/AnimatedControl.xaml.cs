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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace InteractiveTV.PageTransitions
{
    using PageTransitionType = AnimationType;
    using PageTransition = AnimatedControl;
    using System.Windows.Media.Animation;
    using System.Diagnostics;

    /// <summary>
    /// Interaction logic for AnimatedControl.xaml
    /// </summary>
    public partial class AnimatedControl : UserControl
    {
        private Stack<UserControl> pages = new Stack<UserControl>();
        public UserControl CurrentPage { get; set; }

        public static readonly DependencyProperty TransitionTypeProperty = DependencyProperty.Register("TransitionType",
            typeof(AnimationType), typeof(AnimatedControl), new PropertyMetadata(AnimationType.SlideAndFade));

        public PageTransitionType TransitionType
        {
            get { return (PageTransitionType)GetValue(TransitionTypeProperty); }
            set { SetValue(TransitionTypeProperty, value); }
        }

        public AnimatedControl()
        {
            InitializeComponent();
        }

        public void ShowPage(UserControl newPage)
        {
            pages.Push(newPage);
            Task.Factory.StartNew(() => ShowNewPage());
        }

        void ShowNewPage()
        {
            Dispatcher.Invoke((Action) delegate
            {
                if (_ContentPresenter.Content != null)
                {
                    UserControl oldPage = _ContentPresenter.Content as UserControl;

                    if (oldPage != null)
                    {
                        oldPage.Loaded -= OnNewPageLoaded;
                        UnloadPage(oldPage);
                    }
                }
                else
                {
                    ShowNextPage();
                }

            });
        }

        void ShowNextPage()
        {
            Debug.WriteLine("ShowNextPage");
            UserControl newPage = pages.Pop();
            newPage.Loaded += OnNewPageLoaded;
            _ContentPresenter.Content = newPage;
        }

        void newPage_IsVisibleChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            Debug.WriteLine("OnNewPageLoaded");
        }

        void UnloadPage(UserControl page)
        {
            Debug.WriteLine("UnloadPage");
            Storyboard hidePage = (Resources[string.Format("{0}Out", TransitionType.ToString())] as Storyboard).Clone();

            hidePage.Completed += OnHidePageCompleted;
            hidePage.Begin(_ContentPresenter);
        }

        void OnNewPageLoaded(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("OnNewPageLoaded");
            Storyboard showNewPage = Resources[string.Format("{0}In", TransitionType.ToString())] as Storyboard;
            showNewPage.Begin(_ContentPresenter);
            CurrentPage = sender as UserControl;
        }

        void OnHidePageCompleted(object sender, EventArgs e)
        {
            Debug.WriteLine("OnHidePageCompleted");
            _ContentPresenter.Content = null;
            ShowNextPage();
        }		
    }
}
