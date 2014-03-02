using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JiaoExperiment_1
{
    class Point
    {
        private int junctionId;//交叉口ID
        public int JunctionId
        {
            get { return junctionId; }
        }
        private double timeStamp;//通过交叉口时间
        public double TimeStamp
        {
            get { return timeStamp; }
        }
        public double popularity;

        public Point(int id, double time)
        {
            junctionId = id;
            timeStamp = time;
            popularity = 0;
        }
    }
}
