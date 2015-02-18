using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Windows.Forms.Layout;

namespace MapWinGIS.Demo.Helpers
{
    class CustomLayoutEngine : LayoutEngine
    {
        private static int count = 0;
        
        public override bool Layout(object container, LayoutEventArgs layoutEventArgs)
        {
            var parent = container as Control;
            if (parent == null) return false;

            Rectangle parentDisplayRectangle = parent.DisplayRectangle;

            if (parent.Controls.Count == 0)
                return false;

            Control[] source = new Control[parent.Controls.Count];
            parent.Controls.CopyTo(source, 0);

            System.Drawing.Point nextControlLocation = parentDisplayRectangle.Location;

            int localCount = count;
            Debug.WriteLine("Layout " + count + "; " + layoutEventArgs.AffectedProperty);
            count++;
            if (count == 25)
                Debug.Print("");
            foreach (Control c in source)
            {
                if (!c.Visible) continue;

                nextControlLocation.Offset(c.Margin.Left, c.Margin.Top);
                c.Location = nextControlLocation;

                Debug.WriteLine("Location: {0} {1} {2} {3}", c.Left, c.Top, c.Width, c.Text);

                if (c.AutoSize)
                {
                    c.Size = c.GetPreferredSize(parentDisplayRectangle.Size);
                }

                nextControlLocation.Y = parentDisplayRectangle.Y;
                nextControlLocation.X += c.Width /*+ c.Margin.Right */+ parent.Padding.Horizontal + 50;
            }
            Debug.WriteLine("Layout end " + localCount);

            return false;
        }
    }
    
    internal class FlowLayout : LayoutEngine
    {

        // Singleton instance shared by all FlowPanels.
        internal static readonly FlowLayout Instance = new FlowLayout();

        public override bool Layout(object container, LayoutEventArgs layoutEventArgs)
        {
            return base.Layout(container, layoutEventArgs);
        }

        //// Entry point from LayoutEngine
        //internal override bool LayoutCore(object container, LayoutEventArgs args)
        //{

        //    // ScrollableControl will first try to get the layoutbounds from the derived control when
        //    // trying to figure out if ScrollBars should be added.
        //    // VSWhidbey #392913
        //    CommonProperties.SetLayoutBounds(container,
        //        xLayout(container, container.DisplayRectangle, /* measureOnly = */ false));

        //    return CommonProperties.GetAutoSize(container);
        //}

        //// Both LayoutCore and GetPreferredSize forward to this method.  The measureOnly flag determines which
        //// behavior we get.
        //private Size xLayout(object container, Rectangle displayRect, bool measureOnly)
        //{
        //    Size layoutSize = Size.Empty;

        //    if (!wrapContents)
        //    {
        //        // pretend that the container is infinitely wide to prevent wrapping.
        //        // VSWhidbey 161993: displayRectangle.Right is Width + X - subtract X to prevent overflow.
        //        displayRect.Width = Int32.MaxValue - displayRect.X;
        //    }

        //    for (int i = 0; i < container.Children.Count; )
        //    {
        //        int breakIndex;
        //        Size rowSize = Size.Empty;

        //        Rectangle measureBounds = new Rectangle(displayRect.X, displayRect.Y, displayRect.Width,
        //            displayRect.Height - layoutSize.Height);
        //        rowSize = MeasureRow(containerProxy, elementProxy, i, measureBounds, out breakIndex);

        //        if (!measureOnly)
        //        {
        //            Rectangle rowBounds = new Rectangle(displayRect.X,
        //                layoutSize.Height + displayRect.Y,
        //                rowSize.Width,
        //                rowSize.Height);
        //            LayoutRow(containerProxy, elementProxy, /* startIndex = */ i, /* endIndex = */ breakIndex, rowBounds);
        //        }
        //        layoutSize.Width = Math.Max(layoutSize.Width, rowSize.Width);
        //        layoutSize.Height += rowSize.Height;
        //        i = breakIndex;
        //    }
        //}

        //// Just forwards to xLayoutRow.  This will layout elements from the start index to the end index.  RowBounds
        //// was computed by a call to measure row and is used for alignment/boxstretch.  See the ElementProxy class
        //// for an explaination of the elementProxy parameter.
        //private void LayoutRow(ContainerProxy containerProxy, ElementProxy elementProxy, int startIndex, int endIndex,
        //    Rectangle rowBounds)
        //{
        //    int dummy;
        //    Size outSize = xLayoutRow(containerProxy, elementProxy, startIndex, endIndex, rowBounds, /* breakIndex = */
        //        out dummy, /* measureOnly = */ false);
        //    Debug.Assert(dummy == endIndex, "EndIndex / BreakIndex mismatch.");
        //}

        //// Just forwards to xLayoutRow.  breakIndex is the index of the first control not to fit in the displayRectangle.  The
        //// returned Size is the size required to layout the controls from startIndex up to but not including breakIndex.  See
        //// the ElementProxy class for an explaination of the elementProxy parameter.
        //private Size MeasureRow(ContainerProxy containerProxy, ElementProxy elementProxy, int startIndex,
        //    Rectangle displayRectangle, out int breakIndex)
        //{
        //    return xLayoutRow(containerProxy, elementProxy, startIndex, /* endIndex = */
        //        containerProxy.Container.Children.Count, displayRectangle, out breakIndex, /* measureOnly = */ true);
        //}

        //// LayoutRow and MeasureRow both forward to this method.  The measureOnly flag determines which behavior we get.
        //private Size xLayoutRow(ContainerControl containerProxy, ElementProxy elementProxy, int startIndex, int endIndex,
        //    Rectangle rowBounds, out int breakIndex, bool measureOnly)
        //{
        //    Debug.Assert(startIndex < endIndex, "Loop should be in forward Z-order.");
        //    Point location = rowBounds.Location;
        //    Size rowSize = Size.Empty;
        //    int laidOutItems = 0;
        //    breakIndex = startIndex;

        //    bool wrapContents = GetWrapContents(containerProxy.Container);
        //    bool breakOnNextItem = false;


        //    ArrangedElementCollection collection = containerProxy.Controls;
        //    for (int i = startIndex; i < endIndex; i++, breakIndex++)
        //    {
        //        elementProxy.Element = collection[i];

        //        if (!elementProxy.ParticipatesInLayout)
        //        {
        //            continue;
        //        }

        //        // Figure out how much space this element is going to need (requiredSize)
        //        //
        //        Size prefSize;
        //        if (elementProxy.AutoSize)
        //        {
        //            Size elementConstraints = new Size(Int32.MaxValue,
        //                rowBounds.Height - elementProxy.Margin.Size.Height);
        //            if (i == startIndex)
        //            {
        //                // If the element is the first in the row, attempt to pack it to the row width.  (If its not 1st, it will wrap
        //                // to the first on the next row if its too long and then be packed if needed by the next call to xLayoutRow).
        //                elementConstraints.Width = rowBounds.Width - rowSize.Width - elementProxy.Margin.Size.Width;
        //            }

        //            // Make sure that subtracting the margins does not cause width/height to be <= 0, or we will
        //            // size as if we had infinite space when in fact we are trying to be as small as possible.
        //            elementConstraints = LayoutUtils.UnionSizes(new Size(1, 1), elementConstraints);
        //            prefSize = elementProxy.GetPreferredSize(elementConstraints);
        //        }
        //        else
        //        {
        //            // If autosizing is turned off, we just use the element's current size as its preferred size.
        //            prefSize = elementProxy.SpecifiedSize;

        //            // VSWhidbey 406227
        //            // except if it is stretching - then ignore the affect of the height dimension.
        //            if (elementProxy.Stretches)
        //            {
        //                prefSize.Height = 0;
        //            }

        //            // Enforce MinimumSize
        //            if (prefSize.Height < elementProxy.MinimumSize.Height)
        //            {
        //                prefSize.Height = elementProxy.MinimumSize.Height;
        //            }
        //        }
        //        Size requiredSize = prefSize + elementProxy.Margin.Size;

        //        // Position the element (if applicable).
        //        //
        //        if (!measureOnly)
        //        {
        //            // If measureOnly = false, rowBounds.Height = measured row hieght
        //            // (otherwise its the remaining displayRect of the container)

        //            Rectangle cellBounds = new Rectangle(location, new Size(requiredSize.Width, rowBounds.Height));

        //            // We laid out the rows with the elementProxy's margins included.
        //            // We now deflate the rect to get the actual elementProxy bounds.
        //            cellBounds = LayoutUtils.DeflateRect(cellBounds, elementProxy.Margin);

        //            AnchorStyles anchorStyles = elementProxy.AnchorStyles;
        //            containerProxy.Bounds = LayoutUtils.AlignAndStretch(prefSize, cellBounds, anchorStyles);
        //        }

        //        // Keep track of how much space is being used in this row
        //        //

        //        location.X += requiredSize.Width;



        //        if (laidOutItems > 0)
        //        {
        //            // If control does not fit on this row, exclude it from row and stop now.
        //            //   Exception: If row is empty, allow this control to fit on it. So controls
        //            //   that exceed the maximum row width will end up occupying their own rows.
        //            if (location.X > rowBounds.Right)
        //            {
        //                break;
        //            }

        //        }
        //        // Control fits on this row, so update the row size.
        //        //   rowSize.Width != location.X because with a scrollable control
        //        //   we could have started with a location like -100.

        //        rowSize.Width = location.X - rowBounds.X;

        //        rowSize.Height = Math.Max(rowSize.Height, requiredSize.Height);

        //        // check for line breaks.
        //        if (wrapContents)
        //        {
        //            if (breakOnNextItem)
        //            {
        //                break;
        //            }
        //            else if (i + 1 < endIndex && CommonProperties.GetFlowBreak(elementProxy.Element))
        //            {
        //                if (laidOutItems == 0)
        //                {
        //                    breakOnNextItem = true;
        //                }
        //                else
        //                {
        //                    breakIndex++;
        //                    break;
        //                }
        //            }
        //        }
        //        ++laidOutItems;
        //    }

        //    return rowSize;
        //}
    }
}
