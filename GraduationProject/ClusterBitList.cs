using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class ClusterBitList
    {
        //位数组
        private BitArray objectList;
        public BitArray ObjectList
        {
            get { return objectList; }
        }
        //记录位数组长度，等于物体个数
        static public int bitListLength;
        //构造函数
        public ClusterBitList()
        {
            objectList = new BitArray(bitListLength);
        }
        //构造函数，给定集合（可能是字符串集、物体集或聚类结果）构造位集合
        public ClusterBitList(List<string> cluster)
        {
            objectList = new BitArray(bitListLength);
            foreach (string name in cluster)
            {
                int index = Program.cabNameList.IndexOf(name);
                if (index != -1)
                {
                    if (!objectList[index])
                    {
                        objectList[index] = true;
                    }
                    else
                    {
                        Console.WriteLine("第" + index + "个物体出现两次，出错！");
                    }
                }
                else
                {
                    Console.WriteLine("给定cabName未找到，出错！");
                }
            }
        }
        public ClusterBitList(List<CabMetaData> cluster)
        {
            objectList = new BitArray(bitListLength);
            foreach (CabMetaData cab in cluster)
            {
                int index = Program.cabNameList.IndexOf(cab.CabName);
                if (index != -1)
                {
                    if (!objectList[index])
                    {
                        objectList[index] = true;
                    }
                    else
                    {
                        Console.WriteLine("第" + index + "个物体出现两次，出错！");
                    }
                }
                else
                {
                    Console.WriteLine("给定cabName未找到，出错！");
                }
            }
        }
        public ClusterBitList(DBscanCluster cluster)
        {
            objectList = new BitArray(bitListLength);
            foreach (string name in cluster.CabNameList)
            {
                int index = Program.cabNameList.IndexOf(name);
                if (index != -1)
                {
                    if (!objectList[index])
                    {
                        objectList[index] = true;
                    }
                    else
                    {
                        Console.WriteLine("在类" + cluster.ClusterId + "中，第" + index + "个物体出现两次，出错！");
                    }
                }
                else
                {
                    Console.WriteLine("给定cabName未找到，出错！");
                }
            }
        }
        //找到给定名称的物体，返回所在位的值
        public bool FindObject(string name)
        {
            int index = Program.cabNameList.IndexOf(name);
            if (index != -1)
            {
                return objectList[index];
            }
            else
            {
                Console.WriteLine("不存在" + name + ",出错！");
                return false;
            }
        }
        //找到给定名称的物体，修改所在位的值
        public void ChangeObjectStatus(string name)
        {
            int index = Program.cabNameList.IndexOf(name);
            if (index != -1)
            {
                if (objectList[index])
                {
                    objectList[index] = false;
                }
                else
                {
                    objectList[index] = true;
                }
            }
            else
            {
                Console.WriteLine("不存在" + name + ",出错！");
            }
        }
        //两集合求交集，返回交集中元素个数
        static public double IntersectBetweenClusters(ClusterBitList cluster1, ClusterBitList cluster2)
        {
            BitArray b3 = new BitArray(cluster1.objectList);
            BitArray b4 = new BitArray(cluster2.objectList);
            BitArray result = new BitArray(b3.And(b4));
            double resultCount = 0;
            foreach (bool bit in result)
            {
                if (bit)
                {
                    resultCount++;
                }
            }
            return resultCount;
        }
        //两集合求并集，返回并集中元素个数
        static public double UnionBetweenClusters(ClusterBitList cluster1, ClusterBitList cluster2)
        {
            BitArray b3 = new BitArray(cluster1.objectList);
            BitArray b4 = new BitArray(cluster2.objectList);
            BitArray result = new BitArray(b3.Or(b4));
            double resultCount = 0;
            foreach (bool bit in result)
            {
                if (bit)
                {
                    resultCount++;
                }
            }
            return resultCount;
        }

        //检查函数
        public int GetCount(){
            int resultCount = 0;
            foreach (bool bit in objectList)
            {
                if (bit)
                {
                    resultCount++;
                }
            }
            return resultCount;
        }
    }
}
