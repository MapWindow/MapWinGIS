using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace GridTesting.Forms
{
    public class EnumWrapper
    {
        public int Value { get; set; }
        public string Name { get; set; }

        public static List<EnumWrapper> GetItems(Type enumType)
        {
            var values = Enum.GetValues(enumType);
            var names = Enum.GetNames(enumType);

            List<EnumWrapper> list = new List<EnumWrapper>(values.Length);
            for (int i = 0; i < values.Length; i++)
            {
                list.Add(new EnumWrapper() { Name = names[i], Value = (int)values.GetValue(i) });
            }
            return list;
        }

        
    }
    public static class ComboExt
    {
        public static void SetEnum(this ComboBox combo, Type enumType)
        {
            var list = EnumWrapper.GetItems(enumType);
            combo.DisplayMember = "Name";
            combo.ValueMember = "Value";
            combo.DataSource = list;
        }
    }
}
