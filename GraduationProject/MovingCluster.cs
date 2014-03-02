using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace GraduationProject
{
    class MovingCluster
    {
        private SortedList<long, DBscanCluster> clusterList;
        public SortedList<long, DBscanCluster> ClusterList
        {
            get
            {
                return clusterList;
            }
        }
        private DBscanCluster currentCluster;
        public DBscanCluster CurrentCluster
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

        //构造函数
        public MovingCluster(DBscanCluster cluster, long timeStamp, double theta)
        {
            this.clusterList = new SortedList<long, DBscanCluster>();
            this.clusterList.Add(timeStamp, cluster);
            currentCluster = cluster;
            startTimeStamp = timeStamp;
            currentTimeStamp = timeStamp;
            extended = true;
        }
        //复制构造函数
        public MovingCluster(MovingCluster g)
        {
            this.clusterList = new SortedList<long, DBscanCluster>();
            foreach (KeyValuePair<long, DBscanCluster> pair in g.clusterList)
            {
                this.clusterList.Add(pair.Key, pair.Value);
            }
            this.currentCluster = g.CurrentCluster;
            this.startTimeStamp = g.startTimeStamp;
            this.currentTimeStamp = g.currentTimeStamp;
            this.extended = true;
        }
        //从当前cluster中随机返回一个物体的名称
        public string GetRandomObject()
        {
            Random rd = new Random();
            return currentCluster.PointsList[rd.Next(currentCluster.PointsList.Count)].CabName;
        }
        //添加一个cluster
        public void UpdateCluster(long timeStamp, DBscanCluster cluster)
        {
            clusterList.Add(timeStamp, cluster);
            currentTimeStamp = timeStamp;
            currentCluster = cluster;
        }


        //MC2方法使用：求出当前moving cluster中和给定集合的交集元素个数
        public int CalculateIntersectionCount(List<CabMetaData> c)
        {
            List<String> cabNameFromMovingCluster = currentCluster.CabNameList;
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
            List<String> cabNameFromMovingCluster = currentCluster.CabNameList;
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

        //MC2方法使用：给定新增集合，判断是否继续满足moving cluster条件，即两者之间共有的元素是否超过一定比例（theta）；使用位数组进行判断
        public bool IsMovingClusterByBitList(DBscanCluster cluster)
        {
            double intersection = ClusterBitList.IntersectBetweenClusters(this.currentCluster.CabInOrNotList, cluster.CabInOrNotList);
            double union = ClusterBitList.UnionBetweenClusters(this.currentCluster.CabInOrNotList, cluster.CabInOrNotList);
            if (intersection / union >= theta)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        //MC2方法使用：输出该moving cluster至文件
        public int Output(int count)
        {
            if (clusterList.Count >= 2)
            {
                //Console.WriteLine(count + " is a good example; " + clusterList.Count);
                /*
                StreamWriter sw = new StreamWriter("MC"+count + ".txt", false);
                foreach (KeyValuePair<long, DBscanCluster> pair in clusterList)
                {
                    foreach (CabMetaData point in pair.Value.PointsList)
                    {
                        //sw.WriteLine(point.CabName + " " + point.Latitude + " " + point.Longitude + " " + Program.ConvertToDateTime(point.TimeStamp) + " " + Program.ConvertToDateTime(pair.Key));
                        sw.WriteLine(point.CabName + " " + Program.ConvertToDateTime(point.TimeStamp) + " " + pair.Key);
                    }
                    sw.WriteLine("");
                }
                sw.Close();
                */
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}
