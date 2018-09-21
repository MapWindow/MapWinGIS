using System;
// ReSharper disable ArrangeAccessorOwnerBody
// ReSharper disable DelegateSubtraction
// ReSharper disable PossibleInvalidCastExceptionInForeachLoop
// ReSharper disable CheckNamespace

public static class MapEvents
{
    private static AxMapWinGIS._DMapEvents_MouseDownEventHandler _mouseDownDelegate;
    private static AxMapWinGIS._DMapEvents_MouseMoveEventHandler _mouseMoveDelegate;
    private static AxMapWinGIS._DMapEvents_MouseUpEventHandler _mouseUpDelegate;
    private static AxMapWinGIS._DMapEvents_SelectBoxFinalEventHandler _selectBoxDelegate;
    private static AxMapWinGIS._DMapEvents_ShapeHighlightedEventHandler _shapeHighlightedDelegate;

    #region Attach delegate
    /// <summary>
    /// Attaches handers for all map events
    /// </summary>
    public static void AttachMap(AxMapWinGIS.AxMap axMap1)
    {
        axMap1.MouseDownEvent += delegate(object sender, AxMapWinGIS._DMapEvents_MouseDownEvent e)
        {
            var param = new[] { sender, e };
            Invoke(_mouseDownDelegate, param);
        };

        axMap1.MouseMoveEvent += delegate(object sender, AxMapWinGIS._DMapEvents_MouseMoveEvent e)
        {
            var param = new[] { sender, e };
            Invoke(_mouseMoveDelegate, param);
        };

        axMap1.MouseUpEvent += delegate(object sender, AxMapWinGIS._DMapEvents_MouseUpEvent e)
        {
            var param = new[] { sender, e };
            Invoke(_mouseUpDelegate, param);
        };

        axMap1.SelectBoxFinal += delegate(object sender, AxMapWinGIS._DMapEvents_SelectBoxFinalEvent e)
        {
            var param = new[] { sender, e };
            Invoke(_selectBoxDelegate, param);
        };

        axMap1.ShapeHighlighted += delegate(object sender, AxMapWinGIS._DMapEvents_ShapeHighlightedEvent e)
        {
            var param = new[] { sender, e };
            Invoke(_shapeHighlightedDelegate, param);
        };
    }
    #endregion

    #region Exposing events
    public static event AxMapWinGIS._DMapEvents_MouseDownEventHandler MouseDownEvent
    {
        add { _mouseDownDelegate += value; }
        remove { _mouseDownDelegate -= value; }
    }

    public static event AxMapWinGIS._DMapEvents_MouseMoveEventHandler MouseMoveEvent
    {
        add { _mouseMoveDelegate += value; }
        remove { _mouseMoveDelegate -= value; }
    }

    public static event AxMapWinGIS._DMapEvents_MouseUpEventHandler MouseMoveUp
    {
        add { _mouseUpDelegate += value; }
        remove { _mouseUpDelegate -= value; }
    }

    public static event AxMapWinGIS._DMapEvents_SelectBoxFinalEventHandler SelectBoxFinal
    {
        add { _selectBoxDelegate += value; }
        remove { _selectBoxDelegate -= value; }
    }

    public static event AxMapWinGIS._DMapEvents_ShapeHighlightedEventHandler ShapeHighlighted
    {
        add { _shapeHighlightedDelegate += value; }
        remove { _shapeHighlightedDelegate -= value; }
    }
    #endregion

    #region Clear
    /// <summary>
    /// Clears all the event listeners
    /// </summary>
    public static void Clear()
    {
        if (_mouseDownDelegate != null)
        foreach (AxMapWinGIS._DMapEvents_MouseDownEventHandler del in _mouseDownDelegate.GetInvocationList())
            _mouseDownDelegate -= del;

        if (_mouseMoveDelegate != null)
        foreach (AxMapWinGIS._DMapEvents_MouseMoveEventHandler del in _mouseMoveDelegate.GetInvocationList())
            _mouseMoveDelegate -= del;

        if (_mouseUpDelegate != null)
        foreach (AxMapWinGIS._DMapEvents_MouseUpEventHandler del in _mouseUpDelegate.GetInvocationList())
            _mouseUpDelegate -= del;

        if (_selectBoxDelegate != null)
        foreach (AxMapWinGIS._DMapEvents_SelectBoxFinalEventHandler del in _selectBoxDelegate.GetInvocationList())
            _selectBoxDelegate -= del;

        if (_shapeHighlightedDelegate != null)
        foreach (AxMapWinGIS._DMapEvents_ShapeHighlightedEventHandler del in _shapeHighlightedDelegate.GetInvocationList())
            _shapeHighlightedDelegate -= del;
    }
    #endregion

    /// <summary>
    /// Invokes all the delegates of the specified event
    /// </summary>
    private static void Invoke(Delegate del, object[] param)
    {
        if (del != null)
        {
            Delegate[] list = del.GetInvocationList();
            foreach (Delegate item in list)
            {
                item.DynamicInvoke(param);
            }
        }
    }
}

