using System.Collections.Generic;

namespace MapWindow.Legend.Controls.Legend
{
    /// <summary>
    /// Represents binding of shapefile to external database
    /// </summary>
    public class ShapefileBinding
    {
        public List<string> KeyFields;  // names of key fields
        public string DatabaseName = string.Empty;
        public string Guid = string.Empty;             // during saving only

        public ShapefileBinding()
        {
            KeyFields = new List<string>();
        }

        /// <summary>
        /// Gets CSV list of keys
        /// </summary>
        public string CsvKeys
        {
            get
            {
                return string.Join(";", KeyFields.ToArray());
            }
            set
            {
                KeyFields.Clear();
                if (!string.IsNullOrWhiteSpace(value))
                    KeyFields.AddRange(value.Split(new[] {';'}));
            }
        }

        //public bool Empty
        //{
        //    get { return string.IsNullOrWhiteSpace(CsvKeys);  }
        //}

        public bool Imported { get; set; }

        //public string GetWhereString()
        //{
        //    string where = string.Empty;
        //    foreach (var f in this.KeyFields)
        //    {
        //        where += string.Format("[{0}] = @{0} AND ", f);
        //    }
        //    if (where.Length > 0) where = where.Substring(0, where.Length - 4);
        //    return where;
        //}

        public class ParadoxDatabase
        {
            public int Index;
            public string Path;
            public string RelativePath;
        }

        public ParadoxDatabase SourceBinding;
    }
}
