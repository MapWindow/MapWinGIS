// ********************************************************************************************************
// <copyright file="MapWindow.Legend.cs" company="MapWindow.org">
// Copyright (c) MapWindow.org. All rights reserved.
// </copyright>
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http:// Www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
// limitations under the License. 
// 
// The Initial Developer of this version of the Original Code is Sergei Leschinski
// 
// Contributor(s): (Open source contributors should list themselves and their modifications here). 
// Change Log: 
// Date            Changed By      Notes
// ********************************************************************************************************


using System;
using System.Drawing.Drawing2D;
using System.Xml.Serialization;

namespace MapWindow.Legend.Controls.Legend
{
    [Serializable]
    [XmlType("SymbologyPlugin")]
    internal class SymbologySettings
    {
        [XmlAttribute()]
        public MapWinGIS.tkClassificationType CategoriesClassification = MapWinGIS.tkClassificationType.ctNaturalBreaks;
        [XmlAttribute()]
        public int CategoriesCount = 8;
        [XmlAttribute()]
        public bool CategoriesUseGradient = false;
        [XmlAttribute()]
        public bool CategoriesRandomColors = false;
        [XmlElement]
        public ColorBlend CategoriesColorScheme = null;
        [XmlAttribute()]
        public string CategoriesFieldName = "";
        [XmlAttribute()]
        public bool CategoriesVariableSize = false;
        [XmlAttribute()]
        public int CategoriesSizeRange = 8;
        [XmlAttribute()]
        public string CategoriesSortingField = "";

        [XmlAttribute()]
        public bool LabelsGraduatedColors = false;
        [XmlAttribute()]
        public bool LabelsRandomColors = false;
        [XmlAttribute()]
        public bool LabelsVariableSize = false;
        [XmlElement]
        public ColorBlend LabelsScheme = null;
        [XmlAttribute()]
        public int LabelsSchemeIndex = 0;
        [XmlAttribute()]
        public int LabelsSizeRange = 10;
        [XmlAttribute()]
        public int LabelsCategoriesCount = 6;
        [XmlAttribute()]
        public string LabelsFieldName = "";
        [XmlAttribute()]
        public MapWinGIS.tkClassificationType LabelsClassification = MapWinGIS.tkClassificationType.ctNaturalBreaks;
        [XmlAttribute()]
        public int LabelsSize = 10;

        [XmlAttribute()]
        public bool ShowLayerPreview = true;
        [XmlAttribute()]
        public string Comments = "";

        [XmlAttribute()]
        public bool ScaleIcons = false;
        [XmlAttribute()]
        public int IconIndex = -1;
        [XmlAttribute()]
        public string IconCollection = "";
        [XmlAttribute()]
        public bool UpdateMapAtOnce = true;
        [XmlAttribute()]
        public bool ShowQueryValues = true;
        [XmlAttribute()]
        public bool ShowQueryOnMap = false;
    }
}
