#if nsp
namespace MapWinGIS
{
#endif
    using System;

    /// <summary>
    /// Provides methods for random colour generation and colour interpolation based on the specific set of colour breaks.
    /// </summary>
    /// <remarks>
    /// 1. Colour scheme consists of the number of colour breaks defined by a single pair of value and colour. Colour breaks 
    /// are automatically arranged in the increasing order in respect to their values. Therefore the new colour break added with 
    /// ColorScheme.AddBreak won't necessarily be placed in the end of the list. ColorScheme.set_BreakColor changes the colour of a single break, 
    /// while ColorScheme.SetColors (with overloads) provides a convenient shortcut for initialization of colour scheme with several breaks.
    /// \code ColorScheme scheme = new ColorScheme();
    ///
    /// // quick init, adds 2 breaks at once
    /// scheme.SetColors2(tkMapColor.Red, tkMapColor.Yellow);
    /// 
    /// //let's do the same "manually"
    /// scheme.AddBreak(0.0, Convert.ToUInt32(tkMapColor.Red));
    /// scheme.AddBreak(1.0, Convert.ToUInt32(tkMapColor.Yellow));
    /// \endcode \n
    /// 2. ColorScheme.get_RandomColor and ColorScheme.get_GraduatedColor are the central methods to generate 
    /// the random colours within the specified range and to do colour interpolation respectively.\n
    /// \code ColorScheme scheme = some_scheme;
    ///
    /// // what is the colour in the middle between red and yellow?
    /// uint color = scheme.get_GraduatedColor(0.5);
    /// 
    /// // gets some random color between red and yellow
    /// uint color2 = scheme.get_RandomColor(0.5);
    /// \endcode \n
    /// 3. The instance of class can be used to set colors of shapefile categories in fully automatic way (ShapefileCategories.ApplyColorScheme),
    /// as well as for setting the colors of shapefile categories, label categories or chart bars in more controlled, "manual" fashion.</remarks>
    /// \code
    /// // lets' define scheme
    /// ColorScheme scheme = some_scheme;
    ///
    /// // here is shapefile with 8 visualization categories
    /// Shapefile sf = some_shapefile;
    /// sf.Categories.Generate(0, tkClassificationType.ctNaturalBreaks, 8);   // 0 is field index; 8 - number of categories
    ///    
    /// // 1. Let's apply colors automatically
    /// sf.Categories.ApplyColorScheme(tkColorSchemeType.ctSchemeGraduated, scheme);
    ///
    /// // 2. Let's do the same "manually"
    /// sf.Categories.Clear();
    /// for (int i = 0; i < sf.Categories.Count; i++)
    /// {   
    ///     uint color = scheme.get_GraduatedColor((double)(i + 1)/ (double)sf.Categories.Count);  // value between 0.0 and 1.0 as argument
    ///     sf.Categories.get_Item(i).DrawingOptions.FillColor = color;
    /// }
    /// \endcode
    /// \new48 Added in version 4.8
    #if nsp
        #if upd
            public class ColorScheme : MapWinGIS.IColorScheme
        #else        
            public class IColorScheme
        #endif
    #else
        public class ColorScheme
    #endif
    {
        #region IColorScheme Members
        /// <summary>
        /// Adds a single break to the colour scheme.
        /// </summary>
        /// <param name="Value">The value of the break. Must be within 0.0 to 1.0 range.</param>
        /// <param name="Color">The colour of the break.</param>
        public void AddBreak(double Value, uint Color)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the breaks from the colour scheme.
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reverses the order of color breaks in the color scheme.
        /// </summary>
        /// \new494 Added in version 4.9.4
        public void Reverse()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets or set the callback object to report errors.
        /// </summary>
        /// \deprecated v4.9.3 Use GlobalSettings.ApplicationCallback instead.
        public ICallback GlobalCallback
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets or sets the string value associated with the instance of the class.
        /// </summary>
        public string Key
        {
            get { throw new NotImplementedException(); }
            set { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the code of the last error which occurred within this instance of class.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns the number of breaks within colour scheme.
        /// </summary>
        public int NumBreaks
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Removes colour break with specified index.
        /// </summary>
        /// <param name="Index">The index of colour break to remove.</param>
        /// <returns>True on successful removal and false otherwise.</returns>
        public bool Remove(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the existing breaks and creates 2 breaks with the specified colours.
        /// </summary>
        /// <remarks>The values of the breaks will be 0.0 for the first one and 1.0 for the second.</remarks>
        /// <param name="Color1">The colour of the first break.</param>
        /// <param name="Color2">The colour of the second break.</param>
        public void SetColors(uint Color1, uint Color2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the existing breaks and creates 2 breaks with the specified colours.
        /// </summary>
        /// <remarks>The values of the breaks will be 0.0 for the first one and 1.0 for the second.</remarks>
        /// <param name="Color1">The colour of the first break.</param>
        /// <param name="Color2">The colour of the second break.</param>
        public void SetColors2(tkMapColor Color1, tkMapColor Color2)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the existing breaks and creates 2 breaks with the specified colours.
        /// </summary>
        /// <remarks>The values of the breaks will be 0.0 for the first one and 1.0 for the second.</remarks>
        /// <param name="MinRed">The red component of colour for the first colour break (0-255).</param>
        /// <param name="MinGreen">The green component of colour for the first colour break (0-255).</param>
        /// <param name="MinBlue">The blue component of colour for the first colour break (0-255).</param>
        /// <param name="MaxRed">The red component of colour for the second colour break (0-255).</param>
        /// <param name="MaxGreen">The green component of colour for the second colour break (0-255).</param>
        /// <param name="MaxBlue">The blue component of colour for the second colour break (0-255).</param>
        public void SetColors3(short MinRed, short MinGreen, short MinBlue, short MaxRed, short MaxGreen, short MaxBlue)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the existing breaks and creates new set of breaks specified by predefined colour scheme.
        /// </summary>
        /// <remarks>The number of the newly added breaks depends on the colour scheme chosen, usually 2-3.</remarks>
        public void SetColors4(PredefinedColorScheme Scheme)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the colour of the specific colour break.
        /// </summary>
        /// <param name="Index">The index of the colour break.</param>
        /// <returns>The colour of the break.</returns>
        public uint get_BreakColor(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the value of the specific colour break.
        /// </summary>
        /// <param name="Index">The index of the colour break.</param>
        /// <returns>The value of the colour break (0.0-1.0).</returns>
        public double get_BreakValue(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the description of the given error code.
        /// </summary>
        /// <param name="ErrorCode">Error code returned by ColorScheme.LastErrorCode.</param>
        /// <returns>The description of the error.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Calculates the colour which corresponds to the specific value.
        /// </summary>
        /// <remarks>In cases when the value isn't equal to the value of the existing break, colour interpolation will be made.</remarks>
        /// <param name="Value">The value to calculate colour for. Must be within [0.0, 1.0] range.</param>
        /// <returns>The interpolated colour.</returns>
        public uint get_GraduatedColor(double Value)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Generates random colour based on the input value.
        /// </summary>
        /// <remarks>The calculation will be made as following:
        /// - 2 colour breaks will be defined between which the value lies;
        /// - for each component of the colour a random value will be generated which lies between the value of the same colour component for the 2 adjacent breaks;</remarks>
        /// - the resulting colour will be calculated as the sum of 3 components.
        /// .
        /// <param name="Value">The value to calculate colour for. Must be within [0.0, 1.0] range.</param>
        /// <returns>The random colour.</returns>
        public uint get_RandomColor(double Value)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Sets the colour of break with the specified index.
        /// </summary>
        /// <param name="Index">The index of the break.</param>
        /// <param name="retval">The new colour to set.</param>
        public void set_BreakColor(int Index, uint retval)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
#if nsp
}
#endif

