using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace csharp_practice_1
{
    class MovingCluster2
    {
        private SortedList<long, List<CabMetaData>> pointList;
        public SortedList<long, List<CabMetaData>> PointList
        {
            get
            {
                return pointList;
            }
        }
        private List<String> currentCluster;
        public List<String> CurrentCluster
        {
            get
            {
                return currentCluster;
            }
        }
        private long startTimeStamp;
        public long StartTimeStamp
        {
            get
            {
                return startTimeStamp;
            }
        }
        private long currentTimeStamp;
        public long CurrentTimeStamp
        {
            get
            {
                return currentTimeStamp;
            }
        }
        public bool extended;
        public static double theta;

        public MovingCluster2(List<CabMetaData> pointList, long timeStamp, double theta)
        {
            this.pointList = new SortedList<long, List<CabMetaData>>();
            this.pointList.Add(timeStamp, pointList);
            currentCluster = new List<String>(pointList.Count);
            foreach (CabMetaData cabMetaData in pointList)
            {
                currentCluster.Add(cabMetaData.CabName);
            }
            startTimeStamp = timeStamp;
            currentTimeStamp = timeStamp;
            extended = true;
        }
        public MovingCluster2(MovingCluster2 g)
        {
            this.pointList = new SortedList<long, List<CabMetaData>>();
            foreach (KeyValuePair<long, List<CabMetaData>> pair in g.pointList)
            {
                this.pointList.Add(pair.Key, pair.Value);
            }
            this.currentCluster = new List<string>();
            foreach (string str in g.currentCluster)
            {
                this.currentCluster.Add(str);
            }
            this.currentTimeStamp = g.currentTimeStamp;
            this.extended = g.extended;
            this.startTimeStamp = g.startTimeStamp;
        }
        public string GetRandomObject()
        {
            Random rd = new Random();
            return currentCluster[rd.Next(currentCluster.Count)];
        }

        public void UpdateCluster(long timeStamp, List<CabMetaData> c)
        {
            pointList.Add(timeStamp, c);
            currentTimeStamp = timeStamp;
            currentCluster = new List<string>(c.Count);
            foreach (CabMetaData cabMetaData in c)
            {
                currentCluster.Add(cabMetaData.CabName);
            }
            extended = true;
        }
        //MC2方法使用：求出当前moving cluster中和给定集合的交集元素个数
        public int CalculateIntersectionCount(List<CabMetaData> c)
        {
            List<String> cabNameFromMovingCluster = currentCluster;
            List<String> cabNameFromSet = new List<string>(c.Count);
            foreach (CabMetaData cabMetaData in c)
            {
                cabNameFromSet.Add(cabMetaData.CabName);
            }
            IEnumerable<String> intersectionSet = cabNameFromMovingCluster.Intersect(cabNameFromSet);
            return intersectionSet.Count();
        }
        //MC2方法使用：给定新增集合，判断是否继续满足moving cluster条件，即两者之间共有的元素是否超过一定比例（theta）
        public bool IsMovingCluster(List<CabMetaData> c)
        {
            List<String> cabNameFromMovingCluster = currentCluster;
            List<String> cabNameFromSet = new List<string>(c.Count);
            foreach (CabMetaData cabMetaData in c)
            {
                cabNameFromSet.Add(cabMetaData.CabName);
            }
            IEnumerable<String> intersectionSet = cabNameFromMovingCluster.Intersect(cabNameFromSet);
            IEnumerable<String> unionSet = cabNameFromMovingCluster.Union(cabNameFromSet);
            if ((double)intersectionSet.Count() / unionSet.Count() >= theta)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        //MC2方法使用：输出该moving cluster至文件
        public void Output(int count)
        {
            StreamWriter sw = new StreamWriter("MovingCluster" + count + ".txt");
            foreach (KeyValuePair<long, List<CabMetaData>> pair in pointList)
            {
                foreach (CabMetaData point in pair.Value)
                {
                    sw.WriteLine(point.CabName + " " + point.Latitude + " " + point.Longitude + " " + Program.ConvertToDateTime(point.TimeStamp) + " " + Program.ConvertToDateTime(pair.Key));
                }
                sw.WriteLine("");
            }
            if (pointList.Count > 2)
            {
                Console.WriteLine(count + " is a good example; " + pointList.Count);
            }
            sw.Close();
        }
    }
}
