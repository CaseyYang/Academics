using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class NewGrid
    {
        /*注意：
         * 1. 现在的网格设置模式为：输入单元格数，由X/Y总长较长者除以单元格数得到单元网格变长，以单元网格变长为小正方形建立网格；即X/Y中的短边上可能只有较少的单元格数
         * 2. X轴的网格边界存在西经不同表示的问题：当西经用负数表示时，左界小于右界；当西经用绝对值表示时，左界大于右界。具体由数据定！！！（旧金山出租车数据为前一种）
         */
        private static int gridTier;//横竖网格数
        private static double gridLength;//单元网格边长
        private static double gridXLowBound;//网格边界
        private static double gridXUpperBound;
        private static double gridYLowBound;
        private static double gridYUpperBound;

        /*网格（单元格列表）说明：
         * 为方便使用，从X轴、Y轴的下界开始建立单元格列表，即：每个List<Cell>代表一系列X值相同的纵向单元格；
         * 最右/上层的单元格可能不是完整的gridLength*gridLength大小，这点要千万注意，边界判定时最好使用Bound
         */
        private List<List<Cell>> cellList;//网格（单元格列表）

        /*初始化网格相关参数
         * 对四个边界都采取取下/上整
         */
        public static void SetGrid(int gridTier, double gridXLowBound, double gridXUpperBound, double gridYLowBound, double gridYUpperBound)
        {
            NewGrid.gridTier = gridTier;
            NewGrid.gridXLowBound = gridXLowBound < 0 ? 0 - Math.Ceiling(0 - gridXLowBound) : Math.Ceiling(gridXLowBound);
            NewGrid.gridXUpperBound = gridXUpperBound < 0 ? 0 - Math.Ceiling(0 - gridXUpperBound) : Math.Ceiling(gridXUpperBound);
            NewGrid.gridYLowBound = gridYLowBound < 0 ? 0 - Math.Ceiling(0 - gridYLowBound) : Math.Ceiling(gridYLowBound);
            NewGrid.gridYUpperBound = gridYUpperBound < 0 ? 0 - Math.Ceiling(0 - gridYUpperBound) : Math.Ceiling(gridYUpperBound);
            NewGrid.gridLength = Math.Max(gridXUpperBound - gridXLowBound, gridYUpperBound - gridYLowBound) / gridTier;
        }

        public NewGrid(List<CabMetaData> cabLine)
        {
            cellList = new List<List<Cell>>(gridTier);
            for (int i = 0; i < cellList.Capacity; i++)
            {
                cellList.Add(new List<Cell>(gridTier));
                for(int j=0;j<cellList[i].Capacity;j++){
                    cellList[i].Add(new Cell());
                }
            }
            //将每个点分配到网格中
            foreach (CabMetaData cabMetaData in cabLine)
            {
                List<int> index=LocateCell(cabMetaData);
                cellList[index[0]][index[1]].insert(cabMetaData);
            }
        }

        /*范围查询：给定查询点和查询范围，返回所有在查询范围中的网格上的点
         * 方法：首先，根据查询范围得到要访问的网格上下左右界，使界不超过网格范围
         * 然后，遍历在上述界内的所有点，对完全处在查询范围内的单元格，把其中所含的点全部加入结果集；否则，对每个单元格中的点再检查是否在查询范围内
         */
        public static List<CabMetaData> RangeQuery(NewGrid grid,CabMetaData queryPoint, double radious)
        {
            List<CabMetaData> result = new List<CabMetaData>();//结果集

            //获取中心点所在单元格
            //List<int> index = LocateCell(queryPoint);
            //要访问的网格上下左右界
            int down,up,left,right;
            if (queryPoint.Longitude - radious >= gridXLowBound)
            {
                left = (int)((queryPoint.Longitude - radious - gridXLowBound) / gridLength);
            }
            else
            {
                left = 0;
            }
            if (queryPoint.Longitude + radious <= gridXUpperBound)
            {
                right = (int)((queryPoint.Longitude + radious - gridXLowBound) / gridLength);
            }
            else
            {
                right = gridTier - 1;
            }
            if (queryPoint.Latitude - radious >= gridYLowBound)
            {
                down = (int)((queryPoint.Latitude - radious - gridYLowBound) / gridLength);
            }
            else
            {
                down = 0;
            }
            if (queryPoint.Latitude + radious <= gridYUpperBound)
            {
                up = (int)((queryPoint.Latitude + radious - gridYLowBound) / gridLength);
            }
            else
            {
                up = gridTier - 1;
            }
            //遍历所有可能在查询范围内的单元格
            for (int x = left; x <= right; x++)
            {
                for (int y = down; y <= up; y++)
                {
                    //对完全处在查询范围内的单元格，把其中所含的点全部加入结果集
                    if (GetMaxDistBetweenPointAndCell(queryPoint, x, y) <= radious)
                    {
                        result.AddRange(grid.cellList[x][y].points);
                    }
                    else
                    {
                        //对于只有一部分在查询范围内的单元格，对其中的每个点再检查是否在查询范围内
                        foreach (CabMetaData point in grid.cellList[x][y].points)
                        {
                            if (Program.GetDistance(point.Latitude, point.Longitude, queryPoint.Latitude, queryPoint.Longitude) <= radious)
                            {
                                result.Add(point);
                            }
                        }
                    }
                }
            }
            return result;
        }

        private List<int> LocateCell(CabMetaData cabMetaData)
        {
            double x = cabMetaData.Longitude;
            double y = cabMetaData.Latitude;
            List<int> result = new List<int>(2);
            result.Add((int)((x - gridXLowBound) / gridLength));
            result.Add((int)((y - gridYLowBound) / gridLength));
            return result;
            
        }
        /*计算给定点和给定单元格的最大距离
         * 方法：返回给定点和给定单元格四个顶点的距离最大者
         */
        private static double GetMaxDistBetweenPointAndCell(CabMetaData point,int indexX,int indexY)
        {
            double distLeftDown = Program.GetDistance(point.Latitude, point.Longitude, gridYLowBound + indexY * gridLength, gridXLowBound + indexX * gridLength);
            double distLeftUp = Program.GetDistance(point.Latitude, point.Longitude, gridYLowBound + (indexY+1) * gridLength, gridXLowBound + indexX * gridLength);
            double distRightDown = Program.GetDistance(point.Latitude, point.Longitude, gridYLowBound + indexY * gridLength, gridXLowBound + (indexX+1) * gridLength);
            double distRightUp = Program.GetDistance(point.Latitude, point.Longitude, gridYLowBound + (indexY+1) * gridLength, gridXLowBound + (indexX+1) * gridLength);
            return Math.Max(Math.Max(distLeftDown, distLeftUp), Math.Max(distRightDown, distRightUp));
        }

        public static bool Check(NewGrid grid,CabMetaData cab)
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
            int i = 0, j = 0;
            foreach (List<Cell> list in grid.cellList)
            {
                foreach (Cell cell in list)
                {
                    if (Cell.Check(cell, cab))
                    {
                        Console.Write("(" + i + "," + j + ")");
                        return true;
                    }
                    j++;
                }
                i++;
            }
            return false;
        }
    }
}
