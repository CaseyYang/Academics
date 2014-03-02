using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace JiaoExperiment_1
{
    struct StartEnd
    {
        public double start;
        public double end;
        public StartEnd(double start, double end)
        {
            this.start = start;
            this.end = end;
        }
    }
    class TrajInfo
    {
        public int id;
        public int startId;
        public int endId;
        public double popularity;
        public double startTime;
        public double timeInterval;
        public TrajInfo(int id, int start, int end, double popularity,double startTime,double interval)
        {
            this.id = id;
            this.startId = start;
            this.endId = end;
            this.popularity = popularity;
            this.startTime = startTime;
            this.timeInterval = interval;
        }
        public static int Sort(TrajInfo t1, TrajInfo t2)
        {
            return t2.popularity.CompareTo(t1.popularity);
        }
    }
    class Program
    {
        #region 成员变量
        private static int numOfTraj;//轨迹条数
        private static int numOfSegment;//边数量
        private static List<List<Point>> trajSet;//轨迹集
        private static List<Segment> segmentSet;//边集合
        private static double epsilon = 0.6;//时间宽松值
        #endregion

        static void ReadInTrajs()//读入轨迹文件
        {
            StreamReader reader = new StreamReader("trajs.txt");
            numOfTraj = Int32.Parse(reader.ReadLine());
            trajSet = new List<List<Point>>(numOfTraj);
            for (int i = 0; i < numOfTraj; i++)
            {
                int trajLength = Int32.Parse(reader.ReadLine());
                List<Point> traj = new List<Point>(trajLength);
                for (int j = 0; j < trajLength; j++)
                {
                    string[] strList = reader.ReadLine().Split(' ');
                    traj.Add(new Point(Int32.Parse(strList[0]), Double.Parse(strList[1])));
                }
                trajSet.Add(traj);
            }
            reader.Close();
        }

        static void ReadSegments1()//读入经过的每条边的点的信息
        {
            StreamReader reader = new StreamReader("segment1.txt");
            numOfSegment = Int32.Parse(reader.ReadLine());
            segmentSet = new List<Segment>(numOfSegment);
            for (int i = 0; i < numOfSegment; i++)
            {
                string[] strList = reader.ReadLine().Split(' ');
                Segment segment = new Segment(Int32.Parse(strList[0]), Int32.Parse(strList[1]), Int32.Parse(strList[2]));
                for (int j = 0; j < Int32.Parse(strList[2]); j++)
                {
                    segment.points.Add(Double.Parse(strList[j + 3]));
                }
                segmentSet.Add(segment);
            }
            reader.Close();
        }

        static void ReadSegments2()//读入经过的每条边的点组成的时间段的信息
        {
            StreamReader reader = new StreamReader("hotness_dynamic,ts=8.txt");
            numOfSegment = Int32.Parse(reader.ReadLine());
            segmentSet = new List<Segment>(numOfSegment);
            char[] splitChr = new char[] { '-', ':' };
            for (int i = 0; i < numOfSegment; i++)
            {
                string[] strList = reader.ReadLine().Split(' ');
                Segment segment = new Segment(Int32.Parse(strList[0]), Int32.Parse(strList[1]), Int32.Parse(strList[2]));
                for (int j = 0; j < Int32.Parse(strList[2]); j++)
                {
                    string[] timePartStrList = strList[j + 3].Split(splitChr);
                    segment.timeParts.Add(new TimePart(Double.Parse(timePartStrList[0]), Double.Parse(timePartStrList[1]), Double.Parse(timePartStrList[2])));
                }
                segmentSet.Add(segment);
            }
            reader.Close();
        }

        #region 计算轨迹的实际热度
        static int FindSegment(int start, int end)//辅助函数：获得给定起点终点的路段ID
        {
            foreach (Segment segment in segmentSet)
            {
                if (segment.StartId == start && segment.EndId == end || segment.StartId == end && segment.EndId == start)
                {
                    return segment.SegmentId;
                }
            }
            return -1;
        }
        static double CalculateActualPopularity(Segment segment, double time)//辅助函数：计算给定路段的实际热度
        {
            int total = 0;
            foreach (double pointTime in segment.points)
            {
                if (pointTime >= time - epsilon && pointTime <= time + epsilon)
                {
                    total++;
                }
            }
            return total / (2 * epsilon);
        }
        static int GetActualPopularity()//获得轨迹的实际热度，并输出
        {
            List<TrajInfo> outputData = new List<TrajInfo>();
            StreamWriter writer = new StreamWriter("actualpopularity_e="+epsilon+".txt");
            writer.WriteLine(numOfTraj);
            //writer.WriteLine(200);
            //对每条轨迹
            for (int i = 0; i < numOfTraj; i++)
            {
                double trajPopularity = 0;
                //对每个segment
                for (int j = 0; j < trajSet[i].Count - 1; j++)
                {
                    int segmentIndex = FindSegment(trajSet[i][j].JunctionId, trajSet[i][j + 1].JunctionId);
                    if (segmentIndex != -1)
                    {
                        trajSet[i][j].popularity = CalculateActualPopularity(segmentSet[segmentIndex], trajSet[i][j].TimeStamp);
                        trajPopularity += trajSet[i][j].popularity;
                    }
                    else
                    {
                        if (trajSet[i][j].JunctionId != trajSet[i][j + 1].JunctionId)
                        {
                            Console.WriteLine("当轨迹索引为" + i + "、起点ID为" + trajSet[i][j].JunctionId + "时，存在没有信息的交叉口！");
                            return -1;
                        }
                    }
                }
                trajPopularity = trajPopularity / (trajSet[i].Count - 1);
                //只输出轨迹中交叉口数量>6,且<起点，终点>对不重复，前200条按热度降序排列的相关信息：轨迹id（序号） 起点 终点 出发时间 时长
                if (trajSet[i].Count > 6)
                {
                    outputData.Add(new TrajInfo(i,trajSet[i][0].JunctionId,trajSet[i][trajSet[i].Count-1].JunctionId,trajPopularity,trajSet[i][0].TimeStamp,trajSet[i][trajSet[i].Count-1].TimeStamp-trajSet[i][0].TimeStamp));
                }
                writer.WriteLine(i + " " + trajPopularity + " " + trajSet[i][0].JunctionId + " " + trajSet[i][trajSet[i].Count - 1].JunctionId + " " + trajSet[i][0].TimeStamp + " " + (trajSet[i][trajSet[i].Count - 1].TimeStamp - trajSet[i][0].TimeStamp));
                //writer.WriteLine(trajSet[i].Count+" "+trajPopularity);
            }
            //只输出轨迹中交叉口数量>6,且<起点，终点>对不重复，前200条按热度降序排列的相关信息。
            //输出格式：轨迹id（序号） 起点 终点 出发时间 时长
            //outputData.Sort(TrajInfo.Sort);//按给定排序方法TrajInfo.Sort排序
            //int outputCount = 0;//记录已输出的条目数
            //HashSet<StartEnd> startEndSet = new HashSet<StartEnd>();
            //foreach (TrajInfo trajInfo in outputData)
            //{
            //    if (outputCount == 200)
            //    {
            //        break;
            //    }
            //    else
            //    {
            //        StartEnd pair=new StartEnd(trajInfo.startId,trajInfo.endId);
            //        if (startEndSet.Add(pair))
            //        {
            //            writer.WriteLine(trajInfo.id+" "+trajInfo.popularity+" "+trajInfo.startId+" "+trajInfo.endId+" "+trajInfo.startTime+" "+trajInfo.timeInterval);
            //        }
            //    }
            //    outputCount++;
            //}
            writer.Close();
            return 0;
        }
        #endregion

        #region 计算轨迹的动态热度
        static double CalculateDynamicPopularity(Segment segment, double time)//辅助函数：计算给定路段的动态热度
        {
            double total = 0;
            foreach (TimePart timePart in segment.timeParts)
            {
                if (timePart.start <= time && timePart.end >= time)
                {
                    total += timePart.num;
                }
            }
            return total;
        }
        static int GetDynamicPopularity()//获得轨迹的动态热度，并输出
        {
            StreamWriter writer = new StreamWriter("dynamicpopularity_ts=8.txt");
            writer.WriteLine(numOfTraj);
            //对每条轨迹
            for (int i = 0; i < numOfTraj; i++)
            {
                double trajPopularity = 0;
                //对每个segment
                int j = 0;
                for (j = 0; j < trajSet[i].Count - 1; j++)
                {
                    int segmentIndex = FindSegment(trajSet[i][j].JunctionId, trajSet[i][j + 1].JunctionId);
                    if (segmentIndex != -1)
                    {
                        trajSet[i][j].popularity = CalculateDynamicPopularity(segmentSet[segmentIndex], trajSet[i][j].TimeStamp);
                        trajPopularity += trajSet[i][j].popularity;
                    }
                    else
                    {
                        if (trajSet[i][j].JunctionId != trajSet[i][j + 1].JunctionId)
                        {
                            Console.WriteLine("当轨迹索引为" + i + "、起点ID为" + trajSet[i][j].JunctionId + "时，存在没有信息的交叉口！");
                            return -1;
                        }
                    }
                }
                trajPopularity = trajPopularity / (trajSet[i].Count - 1);
                writer.WriteLine(trajPopularity);
            }
            writer.Close();
            return 0;
        }
        #endregion

        #region 实验
        static void OutputExperimentResult()
        {
            StreamReader readerDynamicPopularity = new StreamReader("dynamicpopularity.txt");
            StreamReader readerActualPopularity = new StreamReader("actualpopularity.txt");
            StreamWriter writer = new StreamWriter("experimentresult.txt");
            readerActualPopularity.ReadLine();
            readerDynamicPopularity.ReadLine();
            for (int i = 0; i < numOfTraj; i++)
            {
                double dynamicPopularity = Double.Parse(readerDynamicPopularity.ReadLine());
                double actualPopularity = Double.Parse(readerActualPopularity.ReadLine());
                double error = (dynamicPopularity - actualPopularity) / dynamicPopularity;
                writer.WriteLine(trajSet[i].Count + " " + trajSet[i][0].TimeStamp + " " + epsilon + " " + dynamicPopularity + " " + actualPopularity + " " + error);
            }
            readerActualPopularity.Close();
            readerDynamicPopularity.Close();
            writer.Close();
        }
        #endregion

        static void Main(string[] args)
        {
            ReadInTrajs();
            #region 计算轨迹的实际热度
            //ReadSegments1();
            //GetActualPopularity();
            //for (double e = 0.4; e < 1.2; e = e + 0.2)
            //{
            //    epsilon = e;
            //    GetActualPopularity();
            //}
            #endregion
            #region 计算轨迹的动态热度
            ReadSegments2();
            GetDynamicPopularity();
            #endregion
            #region 实验
            OutputExperimentResult();
            #endregion
        }
    }
}
