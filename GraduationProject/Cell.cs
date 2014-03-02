using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class Cell
    {
        public List<CabMetaData> points;

        public Cell()
        {
            points = new List<CabMetaData>();
        }
        public void insert(CabMetaData metadata)
        {
            points.Add(metadata);
        }
        public List<CabMetaData> GetAllPoints()
        {
            return points;
        }
        public static bool Check(Cell cell, CabMetaData cab)
        {
            if (cell.points.Contains(cab, new CabMetaDataComparer()))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}
