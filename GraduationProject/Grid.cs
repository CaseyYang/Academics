using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    public class Grid
    {
        private static int gridTier;//横竖网格数
        private static double gridLength;//网格边长
        private static double gridXLowBound;//网格边界；注意：这里存在西经不同表示的问题：当西经用负数表示时，左界小于右界；当西经用绝对值表示时，左界大于右界。具体由数据定！！！（旧金山出租车数据为前一种）
        private static double gridXUpperBound;
        private static double gridYLowBound;
        private static double gridYUpperBound;

        private List<Cell> cellList;
        
        public static void SetGrid(int gridTier, double gridLength, double gridXLowBound, double gridXUpperBound, double gridYLowBound, double gridYUpperBound)
        {
            //初始化网格相关参数
            Grid.gridTier = gridTier;
            Grid.gridLength = gridLength;
            Grid.gridXLowBound = gridXLowBound;
            Grid.gridXUpperBound = gridXUpperBound;
            Grid.gridYLowBound = gridYLowBound;
            Grid.gridYUpperBound = gridYUpperBound;
        }

        public Grid(List<CabMetaData> cabLine)
        {
            cellList = new List<Cell>(gridTier * gridTier);
            for (int k = 0; k < cellList.Capacity; k++)
            {
                cellList.Add(new Cell());
            }

            //将每个点分配到网格中
            foreach (CabMetaData cabMetaData in cabLine)
            {
                int gridCellId = LocateCell(cabMetaData);
                cellList[gridCellId].insert(cabMetaData);
            }
        }

        public static List<CabMetaData> RangeQuery(Grid grid, CabMetaData queryPoint, double radious, List<CabMetaData> cabLine)
        {
            List<CabMetaData> result = new List<CabMetaData>();//结果集

            //获取中心点所在单元格，并计算向四个方向最多扩展的次数
            int centerCell = LocateCell(queryPoint);
            int left = centerCell % gridTier;
            int right = gridTier - (centerCell % gridTier) - 1;
            int up = centerCell / gridTier;
            int down = gridTier - (centerCell / gridTier) - 1;

            Queue<CellItem> candidateCells=new Queue<CellItem>();
            bool[] validArray = new bool[gridTier * gridTier];//对应网格是否已入队列，已入为true
            for (int i = 0;i!=gridTier*gridTier ;i++ )
            {
                validArray[i] = false;
            }

            candidateCells.Enqueue(new CellItem(centerCell,0.0));
            validArray[centerCell] = true;
            int times = 1;//扩散次数
            while (candidateCells.Count > 0)
            {
                CellItem nextCell = candidateCells.Dequeue();
                if (nextCell.minDist > radious)//网格完全在圆外面
                {
                    break;
                }
                double cellMaxDist = GetMaxDist(queryPoint, nextCell.cellId);
                List<CabMetaData> pointsOfCell = grid.cellList[nextCell.cellId].GetAllPoints();
                if (cellMaxDist <= radious)//网格完全被圆包含
                {
                    foreach (CabMetaData point in pointsOfCell)
                    {
                        result.Add(point);
                    }
                }
                else//网格与圆部分相交
                {
                    //检查网格每个点是否落于圆中
                    foreach (CabMetaData point in pointsOfCell)
                    {
                        if (Program.GetDistance(queryPoint.Latitude, queryPoint.Longitude, point.Latitude, point.Longitude) <= radious)
                        {
                            result.Add(point);
                        }
                    }
                }
                //扩散邻居网络
                if (candidateCells.Count == 0)
                {
                    if (times <= up)//未上溢出
                    {
                        int upMidCell = centerCell - times * gridTier;
                        if (!validArray[upMidCell])
                        {
                            candidateCells.Enqueue(new CellItem(upMidCell,GetMinDist(queryPoint,upMidCell)));
                        }
                        for (int i = 1; i <= (times < left ? times : left); i++)
                        {
                            int nextCellInt = upMidCell - i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                        for (int i = 1; i <= (times < right ? times : right); i++)
                        {
                            int nextCellInt = upMidCell + i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt,GetMinDist(queryPoint,nextCellInt)));
                            }
                        }
                    }
                    if (times <= down)//未下溢出
                    {
                        int downMidCell = centerCell + times * gridTier;
                        if (!validArray[downMidCell])
                        {
                            candidateCells.Enqueue(new CellItem(downMidCell,GetMinDist(queryPoint,downMidCell)));
                        }
                        for (int i = 1; i <= (times < left ? times : left); i++)
                        {
                            int nextCellInt = downMidCell - i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                        for (int i = 1; i <= (times < right ? times : right); i++)
                        {
                            int nextCellInt = downMidCell + i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                    }
                    if (times <= left)//未左溢出
                    {
                        int leftMidCell = centerCell - times;
                        if (!validArray[leftMidCell])
                        {
                            candidateCells.Enqueue(new CellItem(leftMidCell, GetMinDist(queryPoint, leftMidCell)));
                        }
                        for (int i = 1; i <= (times<up?times:up); i++)
                        {
                            int nextCellInt = leftMidCell - gridTier * i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCell, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                        for (int i = 1; i <= (times<down?times:down); ++i)
                        {
                            int nextCellInt = leftMidCell +gridTier * i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                    }
                    if (times <= right)	//未右溢出
                    {
                        int rightMidCell = centerCell + times;
                        if (!validArray[rightMidCell])
                        {
                            candidateCells.Enqueue(new CellItem(rightMidCell, GetMinDist(queryPoint, rightMidCell)));
                        }
                        for (int i = 1; i <= (times<up?times:up); ++i)
                        {
                            int nextCellInt = rightMidCell - gridTier * i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCell, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                        for (int i = 1; i <= (times<down?times:down); ++i)
                        {
                            int nextCellInt = rightMidCell + gridTier * i;
                            if (!validArray[nextCellInt])
                            {
                                candidateCells.Enqueue(new CellItem(nextCellInt, GetMinDist(queryPoint, nextCellInt)));
                            }
                        }
                    }
                    times++;
                }
            }
            return result;
        }

        private static int LocateCell(CabMetaData cabMetaData)
        {
            double longitude = cabMetaData.Longitude;
            double latitude = cabMetaData.Latitude;
            int cellID = (int)(((gridYUpperBound - latitude) / gridLength) * gridTier) + (int)((longitude - gridXLowBound) / gridLength);
            return cellID;
        }

        private static double GetMaxDist(CabMetaData point,int cellId)
        {
            //获得网格左上角点坐标
            double cellMinX = gridXLowBound + (cellId % gridTier) * gridLength;
            double cellMaxY = gridYUpperBound - (cellId / gridTier) * gridLength;

            //判断与网格相对位置
            if (point.Longitude < cellMinX + gridLength * 0.5)//网格垂直平分线左边
            {
                if (point.Latitude > cellMaxY - gridLength * 0.5)//网格水平平分线上边
                {
                    return Program.GetDistance(point.Latitude,point.Longitude, cellMaxY - gridLength,cellMinX + gridLength);
                }
                else//网格水平平分线下边
                {
                    return Program.GetDistance(point.Latitude,point.Longitude, cellMaxY,cellMinX + gridLength);
                }
            }
            else//网格垂直平分线右边
            {
                if (point.Latitude > cellMaxY - gridLength * 0.5)//网格水平平分线上边
                {
                    return Program.GetDistance(point.Latitude,point.Longitude, cellMaxY - gridLength,cellMinX);
                }
                else//网格水平平分线下边
                {
                    return Program.GetDistance(point.Latitude,point.Longitude,cellMaxY,cellMinX);
                }
            }
        }

        private static double GetMinDist(CabMetaData point, int cellId)
        {
            //获得网格左上角点坐标
            double cellMinX = gridXLowBound + (cellId % gridTier) * gridLength;
            double cellMaxY = gridYUpperBound + (cellId / gridTier) * gridLength;

            //判断与网格相对位置
            if (point.Longitude < cellMinX)//网格左边
            {
                if (point.Latitude > cellMaxY)//左上
                {
                    return Program.GetDistance(point.Latitude, point.Longitude, cellMaxY, cellMinX);
                }
                else
                {
                    if (point.Latitude >= cellMaxY - gridLength)//正左
                    {
                        return Program.GetDistance(point.Latitude, point.Longitude, point.Latitude, cellMinX);
                    }
                    else//左下
                    {
                        return Program.GetDistance(point.Latitude, point.Longitude, cellMaxY - gridLength, cellMinX);
                    }
                }
            }
            else
            {
                if (point.Longitude < cellMinX + gridLength)//网格上中下
                {
                    if (point.Latitude > cellMaxY)//正上
                    {
                        return point.Latitude - cellMaxY;
                    }
                    else
                    {
                        if (point.Latitude >= cellMaxY - gridLength)//正中
                        {
                            return 0;
                        }
                        else//正下
                        {
                            return cellMaxY - gridLength - point.Latitude;
                        }
                    }
                }
                else//网格右边
                {
                    if (point.Latitude > cellMaxY)//右上
                    {
                        return Program.GetDistance(point.Latitude, point.Longitude, cellMaxY, cellMinX + gridLength);
                    }
                    else
                    {
                        if (point.Latitude >= cellMaxY - gridLength)//正右
                        {
                            return Program.GetDistance(point.Latitude, point.Longitude, point.Latitude, cellMinX + gridLength);
                        }
                        else//右下
                        {
                            return Program.GetDistance(point.Latitude,point.Longitude,cellMaxY-gridLength,cellMinX+gridLength);
                        }
                    }
                }
            }
        }
        
        public static bool Check(Grid grid,CabMetaData cab)
        {
            //Console.WriteLine("网格数（单边）："+gridTier);
            //Console.WriteLine("网格变长："+gridLength);
            //Console.WriteLine("网格边界："+gridXLowBound+" "+gridXUpperBound+" "+gridYLowBound+" "+gridYUpperBound);
            //int count = 0;
            //foreach (Cell cell in grid.cellList)
            //{
            //    count+=Cell.Check(cell);
            //}
            //Console.WriteLine(count);
            foreach (Cell cell in grid.cellList)
            {
                if (Cell.Check(cell, cab))
                {
                    return true;
                }
            }
            return false;
        }
    }
}