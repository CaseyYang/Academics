using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace JiaoExperiment_1
{
    struct TimePart{
        public double start;
        public double end;
        public double num;
        public TimePart(double start, double end, double num)
        {
            this.start = start;
            this.end = end;
            this.num = num;
        }
    }
    class Segment
    {
        private int segmentId;//边ID
        public int SegmentId
        {
            get { return segmentId; }
        }
        private int startId;//起点ID
        public int StartId
        {
            get { return startId; }
        }
        private int endId;//终点ID
        public int EndId
        {
            get { return endId; }
        }
        private int numOfPoints;//经过该边的点的数量
        public int NumOfPoints
        {
            get { return numOfPoints; }
        }
        public List<double> points;//经过该边的点序列
        private static int count = 0;//为生成下一个边ID而计数
        public List<TimePart> timeParts;//经过该边的点组成的时间段序列


        public Segment(int start, int end, int num)
        {
            segmentId = count++;
            startId = start;
            endId = end;
            numOfPoints = num;
            points = new List<double>(numOfPoints);
            timeParts = new List<TimePart>(numOfPoints);
        }
    }
}
