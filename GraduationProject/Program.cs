using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Linq;
using System.Collections;
using System.Threading;
using System.Diagnostics;

namespace GraduationProject
{
    class Program
    {
        //出租车数据集
        private static List<Cab> cabList;
        //出租车名称集
        public static List<String> cabNameList;

        //数据集经纬度上下界
        private static double longitudeLowBound;
        private static double longitudeUpperBound;
        private static double latitudeLowBound;
        private static double latitudeUpperBound;

        //时间片参数
        private static long timeStampStart;//时间片序列的起点
        private static int timeSlices;//时间片序列的间隔

        //基于时间片的数据结构
        private static SortedList<long, List<CabMetaData>> timeSliceCabSetList;//按时间片分的出租车数据集，每个List<CabMetaData>是一个时间片数据集
        private static List<NewGrid> timeSliceGridList;//配套使用的网格索引，一个Grid对应timeSliceCabSetList中的一个时间片
        private static List<DBscanClusterSet> timeSliceDBscanList;//对每个时间片的数据集进行DBscan的结果集，每个DBscanClusterSet是一个时间片的DBscan的结果

        //Swarm中所需要的一些参数
        private static int minObject;
        private static int minTimeDuration;
        private static int objectCount;
        private static int timeSliceCount;

        //多线程NewMC方法中需要的一些参数
        private static int threadNum = 2;
        private static int threadCount = 0;
        private static ManualResetEvent eventX;
        private static List<NewMovingCluster> G1;
        private static List<NewMovingCluster> G2;

        #region 读取San Francisco出租车数据(一辆车算一条轨迹)
        private static void ReadSanFranciscoCabData()
        {
            //从索引文件中读取每辆出租车的编号和记录条目数;注意：记录条目数不可信赖！！！
            XmlReader xmlReader = XmlReader.Create(@"D:\Document\subjects\Computer\Develop\Data\SanFransicoCabData\_cabs.txt");
            //XmlReader xmlReader = XmlReader.Create(@"_cabs_example.txt");
            int i = 0;
            int count = 0;
            Console.WriteLine("Start processing index ......");
            while (xmlReader.Read())
            {
                if (xmlReader.NodeType == XmlNodeType.Element && xmlReader.Name == "cab")
                {
                    cabList.Add(new Cab(xmlReader.GetAttribute("id")));
                    cabNameList.Add(xmlReader.GetAttribute("id"));
                }
            }
            xmlReader.Close();
            //读取每辆出租车各自的记录文件
            foreach (Cab cab in cabList)
            {
                i = 0;
                StreamReader fileReader = new StreamReader(@"D:\Document\subjects\Computer\Develop\Data\SanFransicoCabData\new_" + cab.cabName + ".txt");
                while (!fileReader.EndOfStream)
                {
                    string rawStr = fileReader.ReadLine();
                    if (!rawStr.Equals(""))
                    {
                        String[] str = rawStr.Split(' ');
                        cab.cabLine.Add(new CabMetaData(cab.cabName, i++, Convert.ToDouble(str[0]), Convert.ToDouble(str[1]), str[2], Convert.ToInt64(str[3])));
                        UpdateBound(Convert.ToDouble(str[0]), Convert.ToDouble(str[1]));
                    }
                }
                count += i;
                cab.cabLine.Reverse(0, cab.cabLine.Count);
                fileReader.Close();
            }
            Console.WriteLine("All cabs data have been processed. " + count);
        }
        #endregion

        #region 读取San Francisco出租车数据(一次载客算一条轨迹)
        private static void ReadSanFrancisoCabFareTraj()
        {
            //从索引文件中读取每辆出租车的编号和记录条目数;注意：记录条目数不可信赖！！！
            XmlReader xmlReader = XmlReader.Create(@"D:\Document\subjects\Computer\Develop\Data\SanFransicoCabData\_cabs.txt");
            //XmlReader xmlReader = XmlReader.Create(@"_cabs_example.txt");
            int i = 0;
            Console.WriteLine("Start processing index ......");
            while (xmlReader.Read())
            {
                if (xmlReader.NodeType == XmlNodeType.Element && xmlReader.Name == "cab")
                {
                    cabNameList.Add(xmlReader.GetAttribute("id"));
                }
            }
            xmlReader.Close();
            //读取每辆出租车各自的记录文件
            foreach (String cabName in cabNameList)
            {
                i = 0;
                bool flag = false;
                StreamReader fileReader = new StreamReader(@"D:\Document\subjects\Computer\Develop\Data\SanFransicoCabData\new_" + cabName + ".txt");
                while (!fileReader.EndOfStream)
                {
                    string rawStr = fileReader.ReadLine();
                    if (!rawStr.Equals(""))
                    {
                        String[] str = rawStr.Split(' ');
                        if (str[2].Equals("0"))
                        {
                            flag = false;
                            continue;
                        }
                        else
                        {
                            UpdateBound(Convert.ToDouble(str[0]), Convert.ToDouble(str[1]));
                            if (flag == false)
                            {
                                Cab cabTraj = new Cab(cabName);
                                cabTraj.cabLine.Add(new CabMetaData(cabName, i++, Convert.ToDouble(str[0]), Convert.ToDouble(str[1]), str[2], Convert.ToInt64(str[3])));
                                cabList.Add(cabTraj);
                                flag = true;
                            }
                            else
                            {
                                cabList[cabList.Count - 1].cabLine.Add(new CabMetaData(cabName, i++, Convert.ToDouble(str[0]), Convert.ToDouble(str[1]), str[2], Convert.ToInt64(str[3])));
                            }
                        }

                    }
                }
                fileReader.Close();
            }
            foreach (Cab cab in cabList)
            {
                cab.cabLine.Reverse(0, cab.cabLine.Count);
                foreach (CabMetaData point in cab.cabLine)
                {
                    if (point.Fare.Equals("0"))
                    {
                        Console.WriteLine("wrong!");
                        break;
                    }
                }
            }
            Console.WriteLine("All cabs data have been processed, there are " + cabList.Count + " cab trajectories");
        }
        #endregion

        #region 读取水牛数据
        private static void ReadBuffaloData()
        {
            StreamReader fileReader = new StreamReader(@"D:\Document\subjects\Computer\Develop\Data\Kruger Buffalo, VHF Herd Tracking, South Africa.csv");
            fileReader.ReadLine();
            string cabName = "";
            while (!fileReader.EndOfStream)
            {
                string str = fileReader.ReadLine();
                if (!str.Equals(""))
                {
                    string[] strArray = str.Split(',');
                    if (!strArray[10].Equals(cabName))
                    {
                        cabName = strArray[10];
                        cabList.Add(new Cab(strArray[10].Replace(" ", "")));
                        cabNameList.Add(strArray[10].Replace(" ", ""));
                    }
                    cabList[cabList.Count - 1].cabLine.Add(new CabMetaData(strArray[10].Replace(" ", ""), Convert.ToInt32(strArray[0]), Convert.ToDouble(strArray[3]), Convert.ToDouble(strArray[2]), "0", ChangeTimeFormat(strArray[1])));
                }
            }
            fileReader.Close();
            Console.WriteLine(cabList.Count + " buffalos.");
            Console.WriteLine("All cabs data have been processed.");
        }
        private static long ChangeTimeFormat(string str)
        {
            string[] strArray = str.Split(' ');
            string[] strDate = strArray[0].Split('-');
            string[] strTime = strArray[1].Split(':');
            return ConvertToTimeStamp(Convert.ToInt32(strDate[0]), Convert.ToInt32(strDate[1]), Convert.ToInt32(strDate[2]), Convert.ToInt32(strTime[0]), Convert.ToInt32(strTime[1]), Convert.ToInt32(Convert.ToDouble(strTime[2])));
        }
        #endregion

        #region 数据分析函数
        //查看每辆出租车的记录分为几条离散的轨迹
        public static void AnalyzeSeperateRoute()
        {
            int j = 1;
            foreach (Cab cab in cabList)
            {
                Console.WriteLine("Cab No." + j + ":");
                j++;
                int i = 1;
                long form_timestamp = cab.cabLine[0].TimeStamp;
                double form_latitude = cab.cabLine[0].Latitude;
                double form_longitude = cab.cabLine[0].Longitude;
                String form_fare = cab.cabLine[0].Fare;
                foreach (CabMetaData cabMetaData in cab.cabLine)
                {
                    if (cabMetaData.TimeStamp - form_timestamp > 0.5 * 3600 && GetDistance(form_latitude, form_longitude, cabMetaData.Latitude, cabMetaData.Longitude) > 10)
                    {
                        Console.WriteLine(ConvertToDateTime(form_timestamp) + "   " + ConvertToDateTime(cabMetaData.TimeStamp));
                        i++;
                    }
                    form_timestamp = cabMetaData.TimeStamp;
                }
                Console.WriteLine(i + " separate lines.\n");
            }
        }
        //检查距离函数是否正常
        public static void CheckDistanceCorrection()
        {
            int j = 0;
            double form_latitude = 0;
            double form_longitude = 0;
            foreach (CabMetaData cabMetaData in cabList[0].cabLine)
            {
                if (j != 0)
                {
                    Console.WriteLine(GetDistance(form_latitude, form_longitude, cabMetaData.Latitude, cabMetaData.Longitude) + " " + GetDist(form_latitude, form_longitude, cabMetaData.Latitude, cabMetaData.Longitude));
                }
                j++;
                if (j == 100)
                {
                    break;
                }
                form_latitude = cabMetaData.Latitude;
                form_longitude = cabMetaData.Longitude;
            }
        }
        //检查timeSliceCabSetList
        public static void CheckTimeSliceCabSetList()
        {
            Console.WriteLine("共有" + timeSliceCabSetList.Count + "个时间片");
            int j = 0;
            foreach (KeyValuePair<long, List<CabMetaData>> set in timeSliceCabSetList)
            {
                Console.WriteLine("第" + j + "个时间片中有" + set.Value.Count + "个点");
                //for (int i = 0; i < set.Value.Count; i++)
                //{
                //    Console.WriteLine(set.Value[i].cabMetaDataId + " " + set.Value[i].cabName + " " + ConvertToDateTime(set.Value[i].timeStamp));
                //}
                //Console.WriteLine("");
                j++;
            }
        }
        //检查timeSliceGridList
        public static void CheckTimeSliceGridList()
        {
            Console.WriteLine("共有" + timeSliceGridList.Count + "个网格");
            CabMetaData queryPoint = timeSliceCabSetList[timeStampStart][0];
            Console.WriteLine("查询点：(" + queryPoint.Longitude + "," + queryPoint.Latitude + ")");
            List<CabMetaData> result = NewGrid.RangeQuery(timeSliceGridList[0], queryPoint, 0.005);
            foreach (CabMetaData cab in timeSliceCabSetList[timeStampStart])
            {
                if (0.005 >= GetDistance(queryPoint.Latitude, queryPoint.Longitude, cab.Latitude, cab.Longitude))
                {
                    if (!result.Contains(cab, new CabMetaDataComparer()))
                    {
                        if (NewGrid.Check(timeSliceGridList[0], cab))
                        {
                            Console.WriteLine("范围查询有错");
                        }
                        else
                        {
                            Console.WriteLine("网格中根本不存在该点！");
                        }

                    }
                }
                else
                {
                    if (result.Contains(cab, new CabMetaDataComparer()))
                    {
                        Console.WriteLine("wrong1：<" + cab.CabMetaDataId + "," + cab.Longitude + "," + cab.Latitude + ">");
                    }
                }
            }
        }
        //检查timeSliceDBscanList
        public static void CheckDBscanClusterList()
        {
            bool flag = false;
            Console.WriteLine("共有" + timeSliceDBscanList.Count + "个时间片");
            int j = 1;
            foreach (DBscanClusterSet set in timeSliceDBscanList)
            {
                j++;
                if (set.DbscanClusterList.Count == set.ClusterIdList.Count)
                {
                    //Console.WriteLine("第" + j + "个时间片中有" + set.DbscanClusterList.Count + "个类");
                    for (int i = 0; i < set.ClusterIdList.Count; i++)
                    {
                        if (set.DbscanClusterList[i].PointsList.Count != set.DbscanClusterList[i].CabInOrNotList.GetCount())
                        {
                            Console.WriteLine("wrong1");
                            flag = true;
                            break;
                        }
                    }
                    if (flag)
                    {
                        break;
                    }
                    j++;
                }
                else
                {
                    Console.WriteLine("wrong2!");
                    break;
                }
            }
        }
        #endregion

        #region 日期转换
        //把Unix时间戳（1970/1/1距今的秒数）转换为日期时间
        public static String ConvertToDateTime(long timeStamp)
        {
            DateTime dtStart = new DateTime(1970, 1, 1, 0, 0, 0);
            DateTime dtResult = dtStart.Add(new TimeSpan(timeStamp * 10000000));//C#中时间戳刻度为1/10微秒
            return dtResult.ToString();
        }
        //把日期时间转换为Unix时间戳
        public static long ConvertToTimeStamp(int year, int month, int day, int hour, int minute, int second)
        {
            DateTime dtStart = new DateTime(1970, 1, 1, 0, 0, 0);
            DateTime dtNow = new DateTime(year, month, day, hour, minute, second);
            TimeSpan toNow = dtNow.Subtract(dtStart);
            long timeStamp = toNow.Ticks / 10000000;
            return timeStamp;
        }
        #endregion

        #region 距离计算
        //用经纬度计算两点间距离（球面）
        private const double EARTH_RADIUS = 6378.137;//地球半径
        private static double rad(double d)
        {
            return d * Math.PI / 180.0;
        }
        //输入经纬度，计算两点间球面距离，单位为千米
        public static double GetDistance(double lat1, double lng1, double lat2, double lng2)
        {
            double radLat1 = rad(lat1);
            double radLat2 = rad(lat2);
            double a = radLat1 - radLat2;
            double b = rad(lng1) - rad(lng2);
            double s = 2 * Math.Asin(Math.Sqrt(Math.Pow(Math.Sin(a / 2), 2) + Math.Cos(radLat1) * Math.Cos(radLat2) * Math.Pow(Math.Sin(b / 2), 2)));
            s = s * EARTH_RADIUS;
            //s = Math.Round(s * 10000) / 10000;
            return s;
        }
        //传入任意两个点的经纬度坐标，返回其欧氏距离，单位为米
        public static double GetDist(double lat1, double long1, double lat2, double long2)
        {
            double deltaLat = lat1 - lat2;
            double deltaLong = (long2 - long1) * Math.Cos(lat1 * 0.0174532925199432957694);
            return 111226.29021121707545 * Math.Sqrt(deltaLat * deltaLat + deltaLong * deltaLong);
        }
        #endregion

        #region 初始化网格索引
        //初始化网格边界
        private static void InitiateGridBound()
        {
            longitudeLowBound = -180;
            longitudeUpperBound = 180;
            latitudeLowBound = -90;
            latitudeUpperBound = 90;
        }
        //比较给定经纬度是否在已知经纬度上下界内，否则更新已知经纬度
        public static void UpdateBound(double latitude, double longitude)
        {
            if (latitude > latitudeUpperBound)
            {
                latitudeUpperBound = latitude;
            }
            else
            {
                if (latitude < latitudeLowBound)
                {
                    latitudeLowBound = latitude;
                }
            }
            if (longitude > longitudeUpperBound)
            {
                longitudeUpperBound = longitude;
            }
            else
            {
                if (longitude < longitudeLowBound)
                {
                    longitudeLowBound = longitude;
                }
            }
        }
        #endregion

        #region 给定时间段（某天）和时间片间隔，将原始轨迹数据填入时间片集合
        /* 
         * 基本思路：看当前考察的点的时间戳是否在某个时间片的附近，若在，则判断当前点是否是离该时间片最近的点，是则加入该时间片点集，否则遍历下一个点
         * 若不在，增加时间片序数（但其上限不超过当前考察点的时间戳），直至找到当前考察点所在的时间片范围，返回上步
         */
        private static void ProcessData(int year, int month, int day, int hour, int minute, int second, int duration, int timeSlices)//（前5个参数为时间段起点；duration为时间段长度，单位为天；timeSlices为时间片之间相隔时间，单位为秒）
        {
            long timeStampStart = ConvertToTimeStamp(year, month, day, hour, minute, second);   //获得用时间戳表示的时间段起点
            Program.timeStampStart = timeStampStart;
            Program.timeSlices = timeSlices;
            long timeStampEnd = timeStampStart + duration * 86400;                              //时间段终点；时间段长度暂定为1天，即86400秒
            long offset = timeSlices / 3;                                                         //时间片范围的偏移量，单位是秒；暂定为正负3分钟
            long timeStampLowBound = timeStampStart - offset;                                   //时间段起点下限
            long timeStampUpperBound = timeStampEnd + offset;                                   //时间段终点上限
            long currentTimeSlice = timeStampStart;                                             //当前时间片
            long timeBalance = -1;                                                              //记录考察点和时间片的差值
            timeSliceCabSetList = new SortedList<long, List<CabMetaData>>();
            for (int i = 0; timeStampStart + i * timeSlices <= timeStampEnd; i++)
            {
                timeSliceCabSetList.Add(timeStampStart + i * timeSlices, new List<CabMetaData>());
            }
            foreach (Cab cab in cabList)
            {
                CabMetaData formCabMetaData = new CabMetaData();
                currentTimeSlice = timeStampStart;
                timeBalance = -1;
                foreach (CabMetaData cabMetaData in cab.cabLine)
                {
                    if (cabMetaData.TimeStamp < timeStampLowBound)//当前考察点的时间戳还未进入时间段
                    {
                        continue;
                    }
                    else
                    {
                        if (cabMetaData.TimeStamp > timeStampUpperBound)//当前考察点的时间戳离开时间段
                        {
                            if (timeBalance != -1)//可能还有最后一个时间片的数据未处理
                            {
                                timeSliceCabSetList[currentTimeSlice].Add(formCabMetaData);
                                currentTimeSlice += timeSlices;
                                timeBalance = -1;//timeBalance记录数据点时间戳和时间片的差值，同时-1表示当前没有寄存的数据点，和cabLinesForMC2[i].Add必然一起使用
                            }
                            break;
                        }
                        else//当前考察点的时间戳在时间段内
                        {
                            while (cabMetaData.TimeStamp > currentTimeSlice + offset)//考察点的时间戳超过时间片范围上限，增加时间片
                            {
                                if (timeBalance != -1)//可能还有上一个时间片的数据未处理
                                {
                                    timeSliceCabSetList[currentTimeSlice].Add(formCabMetaData);
                                    timeBalance = -1;
                                }
                                currentTimeSlice += timeSlices;
                            }
                            if (cabMetaData.TimeStamp >= currentTimeSlice - offset && cabMetaData.TimeStamp <= currentTimeSlice + offset)//考察点的时间戳在时间片范围内
                            {
                                if (timeBalance == -1 || Math.Abs(currentTimeSlice - cabMetaData.TimeStamp) < timeBalance)//更新离时间片最近的数据点
                                {
                                    timeBalance = Math.Abs(currentTimeSlice - cabMetaData.TimeStamp);
                                    formCabMetaData = new CabMetaData(cabMetaData);
                                }
                                else
                                {
                                    timeSliceCabSetList[currentTimeSlice].Add(formCabMetaData);//把离时间片最近的数据点加入到时间片点集中
                                    timeBalance = -1;
                                    currentTimeSlice += timeSlices;
                                }
                            }
                        }
                    }
                }
            }
            Console.WriteLine(timeSliceCabSetList.Count + " time slices.");
            NewGrid.SetGrid(100, longitudeLowBound, longitudeUpperBound, latitudeLowBound, latitudeUpperBound);
            //double gridLength = Math.Max(longitudeUpperBound - longitudeLowBound, latitudeUpperBound - latitudeLowBound)*1.001 / 100;
            //Grid.SetGrid(100, gridLength, longitudeLowBound, longitudeUpperBound, latitudeLowBound, latitudeUpperBound);
            timeSliceGridList = new List<NewGrid>(timeSliceCabSetList.Count);
            foreach (KeyValuePair<long, List<CabMetaData>> cabSet in timeSliceCabSetList)
            {
                NewGrid grid = new NewGrid(cabSet.Value);
                timeSliceGridList.Add(grid);
            }
            Console.WriteLine(timeSliceGridList.Count + " grids.");
        }
        #endregion

        #region DBSCAN算法
        //初始化DBSCAN算法所必需的一些参数：密度阈值和半径等
        private static void InitiateDbscanParameter()
        {
            DBscan.eps = 0.02;//dbscan中核心点的ε半径
            DBscan.minPts = 5;//dbscan中一个cluster的密度阈值
        }
        //DBSCAN算法见DBscan.ApplyApplyDbscan(Grid,List<CabMetaData>)
        //对每个grid中的数据进行DBSCAN聚类
        private static void DBscanForEachGrid()
        {
            timeSliceDBscanList = new List<DBscanClusterSet>(timeSliceCabSetList.Count);
            int i = 0;
            foreach (KeyValuePair<long, List<CabMetaData>> pair in timeSliceCabSetList)
            {
                DBscanClusterSet dBscanCluster = new DBscanClusterSet(DBscan.ApplyDbscan(timeSliceGridList[i], pair.Value));
                foreach (DBscanCluster c in dBscanCluster.DbscanClusterList)
                {
                    c.Assigned = false;
                    c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
                    c.Matched = false;
                }
                timeSliceDBscanList.Add(dBscanCluster);
                i++;
            }
            //Console.WriteLine("DBscan is over.");
        }
        #endregion DBSCAN算法

        #region 获得Moving Cluster
        //初始化Moving Cluster所需的参数
        private static void InitiateMovingCluster()
        {
            MovingCluster.theta = 0.5;//MC2中判断两个cluster是否为moving cluster的阈值
        }
        //MC1算法
        private static void MC1()
        {
            int count = 0;
            List<MovingCluster> G = new List<MovingCluster>();
            Stopwatch sw = new Stopwatch();
            sw.Start();
            for (int i = 0; i < timeSliceCabSetList.Count; i++)
            {
                foreach (MovingCluster g in G)
                {
                    g.extended = false;
                }
                List<MovingCluster> gNext = new List<MovingCluster>();
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    c.Assigned = false;
                    foreach (MovingCluster g in G)
                    {
                        if (g.IsMovingClusterByBitList(c))
                        {
                            g.extended = true;
                            gNext.Add(new MovingCluster(g));
                            gNext[gNext.Count - 1].UpdateCluster(i, c);
                            c.Assigned = true;
                        }
                    }
                    if (c.Assigned == false)
                    {
                        gNext.Add(new MovingCluster(c, i, MovingCluster.theta));
                    }
                }
                foreach (MovingCluster g in G)
                {
                    if (g.extended == false)
                    {
                        count++;
                        g.Output(count);
                    }
                }
                G = gNext;
            }
            sw.Stop();
            Console.WriteLine("MC1用时：" + sw.Elapsed);
        }
        //MC2算法
        private static long MC2()
        {
            int count = 0;
            int meaningfulCount = 0;
            List<MovingCluster> G = new List<MovingCluster>();
            Stopwatch sw = new Stopwatch();
            sw.Start();
            for (int i = 0; i < timeSliceCabSetList.Count; i++)
            {
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)//timeSliceDBscanList[i]相当于论文伪代码中的L
                {
                    c.Assigned = false;
                }
                List<MovingCluster> gNext = new List<MovingCluster>();
                foreach (MovingCluster g in G)//for (int j = 0; j < G.Count; j++) //foreach (MovingCluster g in G)
                {
                    g.extended = false;
                    int k = (int)((1 - MovingCluster.theta) * g.CurrentCluster.PointsList.Count);
                    HashSet<DBscanCluster> processedDBscanCluster = new HashSet<DBscanCluster>();
                    while (k >= 0)
                    {
                        string o = g.GetRandomObject();//注意：此处随机获取元素必须保证每次获取的是不一样的元素！！！否则会出错！！！！！！
                        DBscanCluster c = new DBscanCluster();
                        foreach (DBscanCluster cabSet in timeSliceDBscanList[i].DbscanClusterList)
                        {
                            if (cabSet.CabNameList.Contains(o))
                            {
                                c = cabSet;
                                break;
                            }
                        }
                        if (c.PointsList.Count == 0 || processedDBscanCluster.Contains(c))
                        {
                            if (c.PointsList.Count == 0)
                            {
                                k--;
                            }
                        }
                        else
                        {
                            int difference = (int)g.CalculateIntersectionCount(c.PointsList);
                            if (g.IsMovingCluster(c.PointsList))
                            {
                                g.extended = true;
                                gNext.Add(new MovingCluster(g));
                                gNext[gNext.Count - 1].UpdateCluster(timeStampStart + i * timeSlices, c);//gNext[gNext.Count - 1].UpdateCluster(timeStampStart + i * timeSlices, c.PointsList);
                                //g.UpdateCluster(timeStampStart + i * timeSlices, c.pointsList);
                                c.Assigned = true;
                            }
                            k = k - difference;
                            processedDBscanCluster.Add(c);
                        }
                    }
                    if (g.extended == false)
                    {
                        count++;
                        meaningfulCount += g.Output(count);
                    }
                }
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    if (c.Assigned == false)
                    {
                        gNext.Add(new MovingCluster(c, timeStampStart + i * timeSlices, MovingCluster.theta));//gNext.Add(new MovingCluster(c.PointsList, timeStampStart + i * timeSlices, MovingCluster.theta));
                    }
                }
                G = gNext;
            }
            foreach (MovingCluster g in G)
            {
                count++;
                meaningfulCount += g.Output(count);
            }
            sw.Stop();
            Console.WriteLine("MC2用时：" + sw.ElapsedMilliseconds + " " + meaningfulCount);
            return sw.ElapsedMilliseconds;
        }
        #endregion

        #region 毕业设计
        //初始化New Moving Cluster所需的参数
        private static void InitiateNewMovingCluster()
        {
            NewMovingCluster.theta1 = 0.5;//NewMC中判断两个cluster是否为moving cluster的阈值
            NewMovingCluster.theta2 = 0.5;//NewMC定义的条件(4)
            NewMovingCluster.maxTimeSpan = 10;//NewMC定义的条件(3)
        }
        //NewMC1算法
        private static long NewMC1()
        {
            int count = 0;
            int meaningfulCount = 0;
            List<NewMovingCluster> G = new List<NewMovingCluster>();
            Stopwatch sw = new Stopwatch();
            sw.Start();
            for (int i = 0; i < timeSliceCabSetList.Count; i++)
            {
                foreach (NewMovingCluster g in G)
                {
                    g.extended = false;
                }
                List<NewMovingCluster> gNext = new List<NewMovingCluster>();
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    c.Assigned = false;
                    foreach (NewMovingCluster g in G)
                    {
                        int difference = (int)g.CalculateIntersectionCount(c.PointsList);
                        if (g.IsMovingCluster(c.PointsList))
                        {
                            g.extended = true;
                            gNext.Add(new NewMovingCluster(g));
                            gNext[gNext.Count - 1].UpdateCluster(i, c);
                            c.Assigned = true;
                        }
                    }
                    if (c.Assigned == false)
                    {
                        gNext.Add(new NewMovingCluster(c, i, NewMovingCluster.theta1));
                    }
                }
                foreach (NewMovingCluster g in G)
                {
                    if (g.extended == false)
                    {
                        if ((i - g.CurrentTimeStamp) <= NewMovingCluster.maxTimeSpan && g.SatisfiedConditionFour(i))
                        {
                            gNext.Add(new NewMovingCluster(g));
                        }
                        else
                        {
                            meaningfulCount += g.Output("1", count);
                        }
                    }
                }
                G = gNext;
            }
            sw.Stop();
            Console.WriteLine("NewMC1用时：" + sw.ElapsedMilliseconds + " " + meaningfulCount);
            return sw.ElapsedMilliseconds;
        }
        //NewMC2算法
        private static long NewMC2()
        {
            int count = 0;
            int meaningfulCount = 0;
            List<NewMovingCluster> G = new List<NewMovingCluster>();
            Hashtable processedCurrentCluster = new Hashtable();
            HashSet<DBscanCluster> processedDBscanCluster = new HashSet<DBscanCluster>();
            Stopwatch sw = new Stopwatch();
            sw.Start();
            for (int i = 0; i < timeSliceCabSetList.Count; i++)
            {
                //foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                //{
                //    c.Assigned = false;
                //    c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
                //    c.Matched = false;
                //}
                List<NewMovingCluster> gNext = new List<NewMovingCluster>();
                /* 由于可能存在有几个new moving cluster在前一时间片中，扩展了同一个cluster（不妨记为A），这些new moving cluster在当前时间片扩展时，完全由A决定，是相同的；
                 * 因此，processedCurrentCluster记录<DBscanCluster,List<DBscanCluster>>键值对，Key值设为A，Value值为可由A扩展的当前时间片中的聚类集合；
                 * 这样，当某一个含有A的moving cluster扩展过后，就会记录在processedCurrentCluster中，之后那些同样含有A的moving cluster就可直接使用
                 */
                //Hashtable processedCurrentCluster = new Hashtable();
                foreach (NewMovingCluster g in G)
                {
                    g.extended = false;
                    #region 如果在processedCurrentCluster中已存在，直接添加
                    if (processedCurrentCluster.ContainsKey(g.CurrentCluster.ClusterId))
                    {
                        foreach (DBscanCluster c in (List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster.ClusterId])
                        {
                            #region 扩展moving cluster
                            g.extended = true;
                            gNext.Add(new NewMovingCluster(g));
                            gNext[gNext.Count - 1].UpdateCluster(i, c);
                            c.Assigned = true;
                            #endregion
                        }
                    }
                    #endregion
                    #region 否则，和当前所有聚类集合比较
                    else
                    {
                        processedCurrentCluster.Add(g.CurrentCluster.ClusterId, new List<DBscanCluster>());
                        int countforo = 0;//枚举一个物体
                        int k = (int)((1 - NewMovingCluster.theta1) * g.CurrentCluster.PointsList.Count);
                        //记录对于该moving cluster，哪些聚类集合已经处理过；防止由于选取的o位于当前时间片中的同一聚类集合中，这样就会导致该moving cluster使用同一个聚类集合多次扩展的错误
                        //HashSet<DBscanCluster> processedDBscanCluster = new HashSet<DBscanCluster>();
                        processedDBscanCluster.Clear();
                        while (k >= 0)
                        {
                            string o = g.CurrentCluster.CabNameList[countforo];
                            countforo++;
                            #region 找出包含o的聚类集合
                            DBscanCluster c = new DBscanCluster();
                            foreach (DBscanCluster cabSet in timeSliceDBscanList[i].DbscanClusterList)
                            {
                                if (cabSet.CabNameList.Contains(o))
                                {
                                    c = cabSet;
                                    break;
                                }
                            }
                            #endregion
                            #region 如果没有找到包含o的聚类集合，k=k-1；如果c在之前已经处理过（见之前注释）；又如果c已经不可能再与更多的new moving cluster扩展，重新枚举一个o
                            if (c.PointsList.Count == 0 || processedDBscanCluster.Contains(c) || c.Matched)
                            {
                                if (c.PointsList.Count == 0 || !processedDBscanCluster.Contains(c))
                                {
                                    k--;
                                }
                            }
                            #endregion
                            else
                            {
                                int difference = (int)ClusterBitList.IntersectBetweenClusters(g.CurrentCluster.CabInOrNotList, c.CabInOrNotList);
                                #region 扩展moving cluster（包括向processedCurrentCluster中添加c）
                                if (g.IsNewMovingClusterByBitList(difference, c))
                                {
                                    g.extended = true;
                                    gNext.Add(new NewMovingCluster(g));
                                    gNext[gNext.Count - 1].UpdateCluster(i, c);
                                    c.Assigned = true;
                                    ((List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster.ClusterId]).Add(c);
                                }
                                #endregion
                                k = k - difference;
                                //UpdateK函数返回c.Matched；若为true，则表示该聚类集合已经不可能被更多的moving cluster用于扩展；反之可能
                                c.UpdateK(difference);
                                processedDBscanCluster.Add(c);
                            }
                        }
                    }
                    #endregion
                    //不满足条件1
                    if (g.extended == false)
                    {
                        count++;
                        //满足条件3且满足条件4
                        if (i - g.CurrentTimeStamp <= NewMovingCluster.maxTimeSpan && g.SatisfiedConditionFour(i))
                        {
                            gNext.Add(new NewMovingCluster(g));
                        }
                        else
                        {
                            meaningfulCount += g.Output("2", count);
                        }
                    }
                }
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    if (c.Assigned == false)
                    {
                        gNext.Add(new NewMovingCluster(c, i, MovingCluster.theta));//gNext.Add(new MovingCluster(c.PointsList, timeStampStart + i * timeSlices, MovingCluster.theta));
                    }
                }
                G = gNext;
            }
            foreach (NewMovingCluster g in G)
            {
                count++;
                meaningfulCount += g.Output("2", count);
            }
            sw.Stop();
            //Console.WriteLine("NewMC2用时：" + sw.ElapsedMilliseconds + " " + meaningfulCount);
            return sw.ElapsedMilliseconds;
        }
        //双线程NewMC算法
        private static int MultiThreadNewMC()
        {
            eventX = new ManualResetEvent(false);
            ThreadPool.QueueUserWorkItem(new WaitCallback(ThreadFunc1));
            ThreadPool.QueueUserWorkItem(new WaitCallback(ThreadFunc2));
            Stopwatch sw = new Stopwatch();
            sw.Start();
            eventX.WaitOne(Timeout.Infinite, true);
            Console.WriteLine("开始合并");
            #region 合并两个线程的结果
            List<NewMovingCluster> result = new List<NewMovingCluster>();
            foreach (NewMovingCluster g1 in G1)
            {
                g1.extended = false;
                foreach (NewMovingCluster g2 in G2)
                {
                    if (g2.extension == "holding")
                    {
                        //满足条件3和条件4
                        if (g2.StartTimeStamp - g1.StartTimeStamp <= NewMovingCluster.maxTimeSpan && (g1.ClusterList.Count + g2.ClusterList.Count) / (g2.CurrentTimeStamp - g1.StartTimeStamp) >= NewMovingCluster.theta2)
                        {
                            DBscanCluster cluster2 = g2.ClusterList[g2.StartTimeStamp];
                            int difference = (int)ClusterBitList.IntersectBetweenClusters(g1.CurrentCluster.CabInOrNotList, cluster2.CabInOrNotList);
                            //满足条件1
                            if (g1.IsNewMovingClusterByBitList(difference, cluster2))
                            {
                                g2.extended = true;
                                g1.extended = true;
                                result.Add(new NewMovingCluster(g1));
                                result[result.Count - 1].JointAnotherNMC(g2);
                            }
                        }
                    }
                    else
                    {
                        result.Add(g2);
                    }
                }
                if (g1.extended == false)
                {
                    result.Add(g1);
                }
            }
            foreach (NewMovingCluster g2 in G2)
            {
                if (g2.extension == "holding" && g2.extended == false)
                {
                    result.Add(g2);
                }
            }
            #endregion
            //TODO：合并操作
            sw.Stop();
            Console.WriteLine(sw.Elapsed);//打印运行时间
            return 0;
        }
        //线程1
        //处理前半段时间片的线程相对简单，和单线程的NewMC相似，只需修改遍历的时间片范围
        private static void ThreadFunc1(Object obj)
        {
            int count = 0;
            G1 = new List<NewMovingCluster>();
            for (int i = 0; i < timeSliceCabSetList.Count / 2 + 1; i++)
            {
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    c.Assigned = false;
                    c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
                    c.Matched = false;
                }
                List<NewMovingCluster> gNext = new List<NewMovingCluster>();
                /* 由于可能存在有几个new moving cluster在前一时间片中，扩展了同一个cluster（不妨记为A），这些new moving cluster在当前时间片扩展时，完全由A决定，是相同的；
                 * 因此，processedCurrentCluster记录<DBscanCluster,List<DBscanCluster>>键值对，Key值设为A，Value值为可由A扩展的当前时间片中的聚类集合；
                 * 这样，当某一个含有A的moving cluster扩展过后，就会记录在processedCurrentCluster中，之后那些同样含有A的moving cluster就可直接使用
                 */
                Hashtable processedCurrentCluster = new Hashtable();
                foreach (NewMovingCluster g in G1)
                {
                    g.extended = false;
                    #region 如果在processedCurrentCluster中已存在，直接添加
                    if (processedCurrentCluster.ContainsKey(g.CurrentCluster))
                    {
                        foreach (DBscanCluster c in (List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster])
                        {
                            #region 扩展moving cluster
                            g.extended = true;
                            gNext.Add(new NewMovingCluster(g));
                            gNext[gNext.Count - 1].UpdateCluster(i, c);
                            c.Assigned = true;
                            #endregion
                        }
                    }
                    #endregion
                    #region 否则，和当前所有聚类集合比较
                    else
                    {
                        processedCurrentCluster.Add(g.CurrentCluster, new List<DBscanCluster>());
                        int countforo = 0;//枚举一个物体
                        int k = (int)((1 - NewMovingCluster.theta1) * g.CurrentCluster.PointsList.Count);
                        //记录对于该moving cluster，哪些聚类集合已经处理过；防止由于选取的o位于当前时间片中的同一聚类集合中，这样就会导致该moving cluster使用同一个聚类集合多刺扩展的错误
                        HashSet<DBscanCluster> processedDBscanCluster = new HashSet<DBscanCluster>();
                        while (k >= 0)
                        {
                            string o = g.CurrentCluster.CabNameList[countforo];
                            countforo++;
                            #region 找出包含o的聚类集合
                            DBscanCluster c = new DBscanCluster();
                            foreach (DBscanCluster cabSet in timeSliceDBscanList[i].DbscanClusterList)
                            {
                                if (cabSet.CabNameList.Contains(o))
                                {
                                    c = cabSet;
                                    break;
                                }
                            }
                            #endregion
                            #region 如果没有找到包含o的聚类集合，k=k-1；如果c在之前已经处理过（见之前注释）；又如果c，重新枚举一个o
                            if (c.PointsList.Count == 0 || processedDBscanCluster.Contains(c) || c.Matched)
                            {
                                if (c.PointsList.Count == 0 || !processedDBscanCluster.Contains(c))
                                {
                                    k--;
                                }
                            }
                            #endregion
                            else
                            {
                                int difference = (int)ClusterBitList.IntersectBetweenClusters(g.CurrentCluster.CabInOrNotList, c.CabInOrNotList);
                                #region 扩展moving cluster（包括向processedCurrentCluster中添加c）
                                if (g.IsNewMovingClusterByBitList(difference, c))
                                {
                                    g.extended = true;
                                    gNext.Add(new NewMovingCluster(g));
                                    gNext[gNext.Count - 1].UpdateCluster(i, c);
                                    c.Assigned = true;
                                    ((List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster]).Add(c);
                                }
                                #endregion
                                k = k - difference;
                                //UpdateK函数返回c.Matched；若为true，则表示该聚类集合已经不可能被更多的moving cluster用于扩展；反之可能
                                c.UpdateK(difference);
                                processedDBscanCluster.Add(c);
                            }
                        }
                    }
                    #endregion
                    //不满足条件1
                    if (g.extended == false)
                    {
                        count++;
                        //满足条件3且满足条件4
                        if (i - g.CurrentTimeStamp <= NewMovingCluster.maxTimeSpan && g.SatisfiedConditionFour(i))
                        {
                            Console.WriteLine("当i= " + i + " 时，有同时满足条件2和条件4的");
                            gNext.Add(new NewMovingCluster(g));
                        }
                        else
                        {
                            g.Output("2", count);
                        }
                    }
                }
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    if (c.Assigned == false)
                    {
                        gNext.Add(new NewMovingCluster(c, i, MovingCluster.theta));//gNext.Add(new MovingCluster(c.PointsList, timeStampStart + i * timeSlices, MovingCluster.theta));
                    }
                }
                G1 = gNext;
            }
            Console.WriteLine("线程1结束");
            Interlocked.Increment(ref threadCount);
            if (threadCount == threadNum)
            {
                eventX.Set();
            }
        }
        //线程2
        //除了修改遍历的时间片范围外，还需要考虑在(timeSliceCabSetList.Count/2+minTimeSpan+1)前开始的那些moving cluster，把他们标记为"holding"，以便之后合并处理
        private static void ThreadFunc2(Object obj)
        {
            int count = timeSliceCabSetList.Count / 2 + 1;
            G2 = new List<NewMovingCluster>();
            for (int i = timeSliceCabSetList.Count / 2 + 1; i < timeSliceCabSetList.Count; i++)
            {
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    c.Assigned = false;
                    c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
                    c.Matched = false;
                }
                List<NewMovingCluster> gNext = new List<NewMovingCluster>();
                /* 由于可能存在有几个new moving cluster在前一时间片中，扩展了同一个cluster（不妨记为A），这些new moving cluster在当前时间片扩展时，完全由A决定，是相同的；
                 * 因此，processedCurrentCluster记录<DBscanCluster,List<DBscanCluster>>键值对，Key值设为A，Value值为可由A扩展的当前时间片中的聚类集合；
                 * 这样，当某一个含有A的moving cluster扩展过后，就会记录在processedCurrentCluster中，之后那些同样含有A的moving cluster就可直接使用
                 */
                Hashtable processedCurrentCluster = new Hashtable();
                foreach (NewMovingCluster g in G2)
                {
                    if (g.extension == "holding")
                    {
                        gNext.Add(new NewMovingCluster(g));
                    }
                    else
                    {
                        g.extended = false;
                        #region 如果在processedCurrentCluster中已存在，直接添加
                        if (processedCurrentCluster.ContainsKey(g.CurrentCluster))
                        {
                            foreach (DBscanCluster c in (List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster])
                            {
                                #region 扩展moving cluster
                                g.extended = true;
                                gNext.Add(new NewMovingCluster(g));
                                gNext[gNext.Count - 1].UpdateCluster(i, c);
                                c.Assigned = true;
                                #endregion
                            }
                        }
                        #endregion
                        #region 否则，和当前所有聚类集合比较
                        else
                        {
                            processedCurrentCluster.Add(g.CurrentCluster, new List<DBscanCluster>());
                            int countforo = 0;//枚举一个物体
                            int k = (int)((1 - NewMovingCluster.theta1) * g.CurrentCluster.PointsList.Count);
                            //记录对于该moving cluster，哪些聚类集合已经处理过；防止由于选取的o位于当前时间片中的同一聚类集合中，这样就会导致该moving cluster使用同一个聚类集合多刺扩展的错误
                            HashSet<DBscanCluster> processedDBscanCluster = new HashSet<DBscanCluster>();
                            while (k >= 0)
                            {
                                string o = g.CurrentCluster.CabNameList[countforo];
                                countforo++;
                                #region 找出包含o的聚类集合
                                DBscanCluster c = new DBscanCluster();
                                foreach (DBscanCluster cabSet in timeSliceDBscanList[i].DbscanClusterList)
                                {
                                    if (cabSet.CabNameList.Contains(o))
                                    {
                                        c = cabSet;
                                        break;
                                    }
                                }
                                #endregion
                                #region 如果没有找到包含o的聚类集合，k=k-1；如果c在之前已经处理过（见之前注释）；又如果c，重新枚举一个o
                                if (c.PointsList.Count == 0 || processedDBscanCluster.Contains(c) || c.Matched)
                                {
                                    if (c.PointsList.Count == 0 || !processedDBscanCluster.Contains(c))
                                    {
                                        k--;
                                    }
                                }
                                #endregion
                                else
                                {
                                    int difference = (int)ClusterBitList.IntersectBetweenClusters(g.CurrentCluster.CabInOrNotList, c.CabInOrNotList);
                                    #region 扩展moving cluster（包括向processedCurrentCluster中添加c）
                                    if (g.IsNewMovingClusterByBitList(difference, c))
                                    {
                                        g.extended = true;
                                        gNext.Add(new NewMovingCluster(g));
                                        gNext[gNext.Count - 1].UpdateCluster(i, c);
                                        c.Assigned = true;
                                        ((List<DBscanCluster>)processedCurrentCluster[g.CurrentCluster]).Add(c);
                                    }
                                    #endregion
                                    k = k - difference;
                                    //UpdateK函数返回c.Matched；若为true，则表示该聚类集合已经不可能被更多的moving cluster用于扩展；反之可能
                                    c.UpdateK(difference);
                                    processedDBscanCluster.Add(c);
                                }
                            }
                        }
                        #endregion
                        //不满足条件1
                        if (g.extended == false)
                        {
                            count++;
                            //不满足条件3或满足条件3但不满足条件4
                            if (i - g.CurrentTimeStamp > NewMovingCluster.maxTimeSpan || !g.SatisfiedConditionFour(i))
                            {
                                if (g.StartTimeStamp <= timeSliceCabSetList.Count / 2 + 1 + NewMovingCluster.maxTimeSpan)
                                {
                                    g.extension = "holding";
                                    gNext.Add(new NewMovingCluster(g));
                                }
                                else
                                {
                                    g.Output("2", count);
                                }
                            }
                            //满足条件3且满足条件4
                            else
                            {
                                g.extended = true;
                                gNext.Add(new NewMovingCluster(g));
                            }
                        }
                    }
                }
                foreach (DBscanCluster c in timeSliceDBscanList[i].DbscanClusterList)
                {
                    if (c.Assigned == false)
                    {
                        gNext.Add(new NewMovingCluster(c, i, MovingCluster.theta));//gNext.Add(new MovingCluster(c.PointsList, timeStampStart + i * timeSlices, MovingCluster.theta));
                    }
                }
                G2 = gNext;
            }
            Console.WriteLine("线程2结束");
            Interlocked.Increment(ref threadCount);
            if (threadCount == threadNum)
            {
                eventX.Set();
            }
        }
        #endregion

        #region ObjectGrowth算法
        //初始化ObjectGrowth算法所必需的的一些参数
        private static void InitiateObjectGrowthParameter()
        {
            minObject = 5;
            minTimeDuration = 32;
            objectCount = cabList.Count;
            timeSliceCount = timeSliceCabSetList.Count;
        }
        //找出所有close swarm：ObjectGrowth算法
        private static void ObjectGrowth(List<String> objectSet, List<int> tMax, int indexOfLastCab)
        {
            //Apriori Pruning
            if (tMax.Count < minTimeDuration)
            {
                return;
            }
            //Backward Pruning
            bool forward_closure = false;
            if (BackwardPruning(indexOfLastCab, objectSet, tMax))
            {
                forward_closure = true;
                for (int i = indexOfLastCab + 1; i < cabList.Count; i++)
                {
                    List<string> newObjectSet = new List<string>(objectSet);
                    newObjectSet.Add(cabList[i].cabName);
                    List<int> tMax2 = GenerateMaxTimeset(i, indexOfLastCab, tMax);
                    if (tMax.Count == tMax2.Count) //if (forward_closure && tMax.Count == tMax2.Count)
                    {
                        forward_closure = false;
                    }
                    ObjectGrowth(newObjectSet, tMax2, i);
                }
                if (forward_closure && objectSet.Count >= minObject)
                {
                    OutputSwarm(objectSet, tMax);
                }
            }
        }
        private static bool BackwardPruning(int indexOfLastCab, List<String> objectSet, List<int> tMax)
        {
            for (int i = 0; i < indexOfLastCab; i++)
            {
                if (objectSet.Contains(cabList[indexOfLastCab].cabName))
                {
                    continue;
                }
                if (DBscanCluster.Compare1(cabList[indexOfLastCab].cabName, objectSet, tMax, timeSliceDBscanList))
                {
                    return false;
                }
            }
            return true;
        }
        private static List<int> GenerateMaxTimeset(int indexOfo, int indexOfolast, List<int> tMax)
        {
            List<int> result = new List<int>();
            if (indexOfolast != -1)
            {
                foreach (int i in tMax)
                {
                    foreach (DBscanCluster cluster in timeSliceDBscanList[i].DbscanClusterList)
                    {
                        if (cluster.CabNameList.Contains(cabList[indexOfo].cabName) && cluster.CabNameList.Contains(cabList[indexOfolast].cabName))
                        {
                            result.Add(i);
                            break;
                        }
                    }
                }
            }
            else
            {
                foreach (int i in tMax)
                {
                    foreach (DBscanCluster cluster in timeSliceDBscanList[i].DbscanClusterList)
                    {
                        if (cluster.CabNameList.Contains(cabList[indexOfo].cabName))
                        {
                            result.Add(i);
                            break;
                        }
                    }
                }
            }
            return result;
        }
        private static void OutputSwarm(List<string> listStr, List<int> listT)
        {
            Console.WriteLine("点集个数：" + listStr.Count);
            Console.WriteLine("时间片集个数：" + listT.Count);
            Console.WriteLine(" ");
            StreamWriter fileWriter = new StreamWriter("data" + listStr.GetHashCode() + ".js");
            fileWriter.WriteLine("data = [");
            bool flagForStart = true;
            foreach (int i in listT)
            {
                if (flagForStart)
                {
                    fileWriter.WriteLine("[");
                    flagForStart = false;
                }
                else
                {
                    fileWriter.WriteLine(",\r\n[");
                }
                for (int j = 0; j < listStr.Count; j++)
                {
                    if (j == 0)
                    {
                        foreach (CabMetaData cab in timeSliceCabSetList[timeStampStart + i * timeSlices])
                        {
                            if (cab.CabName.Equals(listStr[j]))
                            {
                                fileWriter.Write("{y:" + cab.Latitude + ",x:" + cab.Longitude + ",t:" + cab.TimeStamp + "}");
                                break;
                            }
                        }
                    }
                    else
                    {
                        foreach (CabMetaData cab in timeSliceCabSetList[timeStampStart + i * timeSlices])
                        {
                            if (cab.CabName.Equals(listStr[j]))
                            {
                                fileWriter.Write(",\r\n{y:" + cab.Latitude + ",x:" + cab.Longitude + ",t:" + cab.TimeStamp + "}");
                                break;
                            }
                        }
                    }
                }
                fileWriter.WriteLine("]");
            }
            fileWriter.WriteLine("]");
            fileWriter.Close();
        }
        #endregion

        #region 输出函数
        //输入一条轨迹，输出成可被Google Maps读取的格式文件
        public static void OutputTraj(StreamWriter sw, List<CabMetaData> cabLine)
        {
            sw.WriteLine("data = [");
            bool flag = false;
            foreach (CabMetaData cabMetaData in cabLine)
            {
                if (flag)
                {
                    sw.WriteLine(",");
                }
                else
                {
                    flag = true;
                }
                sw.Write("{x:" + cabMetaData.Longitude + ",y:" + cabMetaData.Latitude + ",t:" + ConvertToDateTime(cabMetaData.TimeStamp) + "}");
            }
            sw.Write("]");
            sw.Close();
        }
        //输出所有时间片的聚类结果
        public static void OutputTimeSliceDBscanList()
        {
            StreamWriter sw = new StreamWriter("TimeSliceDBscanList.txt", false);
            int i = 0;
            foreach (DBscanClusterSet clusterSet in timeSliceDBscanList)
            {
                sw.WriteLine(i + ": ");
                i++;
                foreach (DBscanCluster cluster in clusterSet.DbscanClusterList)
                {
                    sw.Write("    " + cluster.ClusterId + ": ");
                    foreach (string name in cluster.CabNameList)
                    {
                        sw.Write(name + " ");
                    }
                    sw.WriteLine("");
                }
                sw.WriteLine("");
            }
            sw.Close();
        }
        //输出某辆车的载客轨迹集
        public static void OutputFareTraj()
        {
            StreamWriter sw = new StreamWriter("data.js");
            sw.WriteLine("data = [");
            bool flag = false;
            foreach (Cab cab in cabList)
            {
                if (cab.cabName.Equals(cabNameList[0]))
                {
                    if (flag == false)
                    {
                        flag = true;
                    }
                    else
                    {
                        sw.WriteLine(",");
                    }
                    sw.Write("[");
                    int i = 0;
                    for (i = 0; i < cab.cabLine.Count - 1; i++)
                    {
                        sw.Write("{x:" + cab.cabLine[i].Longitude + ",y:" + cab.cabLine[i].Latitude + ",t:\"" + ConvertToDateTime(cab.cabLine[i].TimeStamp) + "\"},");
                    }
                    sw.WriteLine("{x:" + cab.cabLine[i].Longitude + ",y:" + cab.cabLine[i].Latitude + ",t:\"" + ConvertToDateTime(cab.cabLine[i].TimeStamp) + "\"}]");
                }
                else
                {
                    break;
                }
            }
            sw.WriteLine("]");
            sw.Close();
        }
        #endregion

        static void Main(string[] args)
        {
            //初始化
            cabList = new List<Cab>();
            cabNameList = new List<String>();
            DBscanCluster.idCount = 0;

            InitiateGridBound();
            InitiateDbscanParameter();
            InitiateMovingCluster();
            InitiateNewMovingCluster();

            //读入Los Angeles出租车数据
            ReadSanFrancisoCabFareTraj();
            //读入犀牛数据
            //ReadBuffaloData();

            //ClusterBitList.bitListLength = cabNameList.Count;
            //ProcessData(2002, 1, 1, 0, 0, 0, 540, 24 * 60 * 60);
            //DBscanForEachGrid();
            OutputFareTraj();

            #region 第一张图
            //long qqq = 0;//时间累积和，用以除以次数
            //qqq += MC2();
            //Console.WriteLine("MC2方法平均用时：" + qqq / 1);
            //qqq = 0;

            //for (int i = 0; i < 100; i++)
            //{
            //    qqq += NewMC1();
            //}
            //Console.WriteLine("NewMC1方法平均用时：" + qqq / 100);
            //qqq = 0;

            //for (int i = 0; i < 100; i++)
            //{
            //    //timeSliceDBscanList.Clear();
            //    //DBscanForEachGrid();
            //    for (int j = 0; j < timeSliceCabSetList.Count; j++)
            //    {
            //        foreach (DBscanCluster c in timeSliceDBscanList[j].DbscanClusterList)
            //        {
            //            c.Assigned = false;
            //            c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
            //            c.Matched = false;
            //        }
            //    }
            //    qqq += NewMC2();
            //}
            //Console.WriteLine("NewMC2方法平均用时：" + qqq / 100);
            #endregion

            #region 第二张图
            //MovingCluster.theta = 0.25;
            //NewMovingCluster.theta1 = 0.25;
            //MC2();
            //NewMC1();
            //MovingCluster.theta = 0.5;
            //NewMovingCluster.theta1 = 0.5;
            //MC2();
            //NewMC1();
            //MovingCluster.theta = 0.75;
            //NewMovingCluster.theta1 = 0.75;
            //MC2();
            //NewMC1();
            //MovingCluster.theta = 1;
            //NewMovingCluster.theta1 = 1;
            //MC2();
            //NewMC1();
            #endregion

            #region 第三张图
            //for (int i = 1; i <= 4; i++)
            //{
            //    for (int j = 0; j < timeSliceCabSetList.Count; j++)
            //    {
            //        foreach (DBscanCluster c in timeSliceDBscanList[j].DbscanClusterList)
            //        {
            //            c.Assigned = false;
            //            c.k = (int)((1 - NewMovingCluster.theta1) * c.PointsList.Count);
            //            c.Matched = false;
            //        }
            //    }
            //    NewMovingCluster.theta2 = i * 0.25;
            //    Console.WriteLine(NewMovingCluster.theta2);
            //    NewMC1();
            //}
            #endregion

            #region ObjectGrowth
            //InitiateObjectGrowthParameter();
            //List<int> tMax = new List<int>();
            //for (int i = 0; i < timeSliceCabSetList.Count; i++)
            //{
            //    tMax.Add(i);
            //}
            //cabNameList = new List<string>();
            //ObjectGrowth(cabNameList, tMax, -1);
            #endregion
        }
    }
}