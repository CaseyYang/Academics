using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class DBscan
    {
        public static int minPts = 5;//密度阈值
        public static double eps = 0.005;//ε半径

        //dbscan过程（深度优先方法）
        public static List<DBscanCluster> ApplyDbscan(NewGrid grid, List<CabMetaData> pointsList)
        {
            List<DBscanCluster> resultList = new List<DBscanCluster>();// 存储最后的聚类结果
            foreach (CabMetaData point in pointsList)
            {
                if (!point.extension.Equals(""))
                {
                    continue;
                }
                else
                {
                    List<CabMetaData> neighboursList = NewGrid.RangeQuery(grid, point, eps);
                    if (neighboursList.Count >= minPts)
                    {
                        point.extension = "core";
                        resultList.Add(new DBscanCluster());
                        resultList[resultList.Count - 1].Add(point);
                        Stack<CabMetaData> neighbourStack = new Stack<CabMetaData>();
                        foreach (CabMetaData neightbour in neighboursList)
                        {
                            if (!neightbour.CabMetaDataId.Equals(point.CabMetaDataId))//没必要把查询点也压入栈中（尽管不影响正确性），减少出栈次数
                            {
                                neighbourStack.Push(neightbour);
                            }
                        }
                        while (neighbourStack.Count > 0)
                        {
                            CabMetaData currentPoint = neighbourStack.Pop();
                            if (currentPoint.extension.Equals("") || currentPoint.extension.Equals("noice"))//如果标记为"core"或"neighbour"，说明已经被处理过了
                            {
                                resultList[resultList.Count - 1].Add(currentPoint);
                                if (currentPoint.extension.Equals(""))//如果标记为"noice"，说明肯定不是核心点
                                {
                                    List<CabMetaData> newNeighboursList = NewGrid.RangeQuery(grid, currentPoint, eps);
                                    if (neighboursList.Count >= minPts)//ε邻域大于密度阈值，标记为"core"
                                    {
                                        currentPoint.extension = "core";
                                        foreach (CabMetaData neightbour in newNeighboursList)
                                        {
                                            if (!neightbour.CabMetaDataId.Equals(currentPoint.CabMetaDataId))//没必要把查询点也压入栈中（尽管不影响正确性），减少出栈次数
                                            {
                                                neighbourStack.Push(neightbour);
                                            }
                                        }
                                    }
                                    else//ε邻域小于密度阈值，标记为"neighbour"
                                    {
                                        currentPoint.extension = "neighbour";
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        point.extension = "noice";
                    }
                }
            }
            return resultList;
        }
        //判断是否是核心点
        public static List<CabMetaData> IsKeyPoint(NewGrid grid, List<CabMetaData> lst, CabMetaData point)
        {
            List<CabMetaData> indexList = NewGrid.RangeQuery(grid, point, eps);
            if (indexList.Count >= minPts)
            {
                point.extension = "true";
                return indexList;
            }
            else
            {
                return null;
            }
        }
    }
}
