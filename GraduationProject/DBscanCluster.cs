using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class DBscanCluster
    {
        public static int idCount = 0;//静态成员，用于提供唯一的ClusterId

        private int clusterId;//每个Cluster唯一的Id
        public int ClusterId
        {
            get
            {
                return clusterId;
            }
        }
        private List<CabMetaData> pointsList;//每个Cluster包含的物体列表
        public List<CabMetaData> PointsList
        {
            get
            {
                return pointsList;
            }
        }
        private List<String> cabNameList;//每个Cluster包含的物体名称列表
        public List<string> CabNameList
        {
            get
            {
                return cabNameList;
            }
        }
        private ClusterBitList cabInOrNotList;//每个Cluster包含的物体,以位数组的方式来表示
        public ClusterBitList CabInOrNotList
        {
            get
            {
                return cabInOrNotList;
            }
        }
        private bool assigned;//MC2方法中使用
        public bool Assigned
        {
            get { return assigned; }
            set { assigned = value; }
        }
        private bool matched;
        public bool Matched
        {
            get
            {
                return matched;
            }
            set
            {
                matched = value;
            }
        }
        public int k;

        public DBscanCluster()
        {
            this.pointsList = new List<CabMetaData>();
            this.cabNameList = new List<string>();
            cabInOrNotList = new ClusterBitList();
            assigned = false;
            matched = false;
            k = 0;
            this.clusterId = idCount;
            idCount++;
        }
        public DBscanCluster(List<CabMetaData> pointsList)
        {
            this.pointsList = pointsList;
            this.cabNameList = new List<string>(pointsList.Count);
            this.cabInOrNotList = new ClusterBitList(pointsList);
            foreach (CabMetaData cabMetaData in pointsList)
            {
                this.cabNameList.Add(cabMetaData.CabName);
            }
            Assigned = false;
            matched = false;
            k = 0;
            this.clusterId = idCount;
            idCount++;
        }
        public void Add(CabMetaData point)
        {
            pointsList.Add(point);
            cabNameList.Add(point.CabName);
            point.clusterId = clusterId;
            cabInOrNotList.ChangeObjectStatus(point.CabName);
        }
        public bool UpdateK(int difference)
        {
            if (k >= 0)
            {
                k = k - difference;
            }
            matched = (k < 0);
            return matched;
        }
        //public void Remove(CabMetaData point)
        //{
        //    pointsList.Remove(point);
        //    cabNameList.Remove(point.CabName);
        //    point.clusterId = -1;
        //}
        public static bool Compare1(string cabName, List<string> cabNameList, List<int> tMax, List<DBscanClusterSet> timeSliceDBscanList)//cabName相当于伪代码中的o集合；cabNameList相当于伪代码中的O集合
        {
            bool flag0 = true;//表示对tMax中的每个时间片，含有o元素的类的集合是否为含有O中所有元素的类的集合的子集
            foreach (int i in tMax)
            {
                DBscanClusterSet c1 = new DBscanClusterSet();//c1相当于伪代码中Ct(o)集合，即t时间片中所有包含o的类的集合
                foreach (DBscanCluster cluster in timeSliceDBscanList[i].DbscanClusterList)
                {
                    if (cluster.cabNameList.Contains(cabName))
                    {
                        c1.Add(cluster);
                    }
                }
                bool flag1 = true;//用来表示c2是否因为刚刚新建而空，因为若c2是刚刚新建而为空，则不能和cTmp集合进行交集操作
                DBscanClusterSet c2 = new DBscanClusterSet();//c2相当于伪代码中Ct(O)集合，即t时间片中所有包含O中每个元素的类的集合
                foreach (string strName in cabNameList)
                {
                    DBscanClusterSet cTmp = new DBscanClusterSet();//cTmp表示t时间片中所有包含元素strName的类的集合
                    foreach (DBscanCluster cluster in timeSliceDBscanList[i].DbscanClusterList)
                    {
                        if (cluster.cabNameList.Contains(strName))
                        {
                            cTmp.Add(cluster);
                        }
                    }
                    if (flag1)//若c2是新建，则用cTmp直接填充c2
                    {
                        c2 = cTmp;
                        flag1 = false;
                        //flag1 = true;
                    }
                    else//否则求cTmp和c2的交集，即包含O中所有已遍历元素的类的集合
                    {
                        DBscanClusterSet cTmp2 = new DBscanClusterSet(c2.DbscanClusterList.Intersect(cTmp.DbscanClusterList, new DBscanClusterComparer()).ToList());
                        c2 = cTmp2;
                    }
                }
                //flag1 = true;//用来表示c1是否是c2的子集
                foreach (int id in c1.ClusterIdList)
                {
                    if (!c2.ClusterIdList.Contains(id))//至少一个c1中的id不存在于c2集合中，因此c1不是c2的子集
                    {
                        return false;
                        //flag1 = false;
                        //break;
                    }
                }
                //if (!flag1)
                //{
                //    flag0 = false;
                //    break;
                //}
            }
            return true;
            //return flag0;
        }
        public static List<int> Compare2(string str1, string str2, List<DBscanClusterSet> clusterLlist)
        {
            List<int> result = new List<int>();
            for (int i = 0; i < clusterLlist.Count; i++)
            {
                foreach (DBscanCluster cluster in clusterLlist[i].DbscanClusterList)
                {
                    if (cluster.cabNameList.Contains(str1) && cluster.cabNameList.Contains(str2))
                    {
                        result.Add(i);
                        break;
                    }
                }
            }
            return result;
        }
        //修改指定cabName所代表的位
    }

    //定义两个cluster之间是否相同的方法，用于intersection等操作
    class DBscanClusterComparer : IEqualityComparer<DBscanCluster>
    {
        public bool Equals(DBscanCluster c1, DBscanCluster c2)
        {
            if (c1.ClusterId == c2.ClusterId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        public int GetHashCode(DBscanCluster c)
        {
            return c.ClusterId.GetHashCode();
        }
    }
}