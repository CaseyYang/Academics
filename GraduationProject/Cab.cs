using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    public class Cab
    {
        public Cab(String cabname)
        {
            this.cabName = cabname;
            this.cabLine = new List<CabMetaData>();
        }
        public String cabName;                  //出租车名称
        public int updatesCount;                //数据条目数
        public List<CabMetaData> cabLine;       //数据集
    }
}
