using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class CellItem
    {
        public int cellId;
        public double minDist;

        public CellItem(int id,double minDist){
            this.cellId = id;
            this.minDist = minDist;
        }
        public CellItem(CellItem cellItem,double minDist)
        {
            this.cellId = cellItem.cellId;
            this.minDist = minDist;
        }
    }
}
