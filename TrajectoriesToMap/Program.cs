using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

namespace TrajectoriesToMap
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

        /// <summary>
        /// 初始化网格边界
        /// </summary>
        private static void InitiateGridBound()
        {
            longitudeLowBound = 180;
            longitudeUpperBound = -180;
            latitudeLowBound = 90;
            latitudeUpperBound = -90;
        }
        /// <summary>
        /// 比较给定经纬度是否在已知经纬度上下界内，否则更新已知经纬度
        /// </summary>
        /// <param name="latitude">给定纬度</param>
        /// <param name="longitude">给定经度</param>
        public static void UpdateBoundForDrawMap(double latitude, double longitude)
        {
            //全局图的范围：纬度37.3~50；经度-122.75~-116 放大系数为10000
            //downtown1图的范围：纬度37.7~37.82；经度-122.53~-122.38 放大系数为50000
            //downtown2图的范围：纬度37.78~37.8；经度-122.4223~-122.3923 放大系数100000
            if (InBound(latitude, longitude))
            {
                if (latitude > latitudeUpperBound)
                {
                    latitudeUpperBound = latitude;
                }
                if (latitude < latitudeLowBound)
                {
                    latitudeLowBound = latitude;
                }
                if (longitude > longitudeUpperBound)
                {
                    longitudeUpperBound = longitude;
                }
                if (longitude < longitudeLowBound)
                {
                    longitudeLowBound = longitude;
                }
            }
        }
        public static bool InBound(double latitude, double longitude)
        {
            return latitude >= 37.7 && latitude < 37.82 && longitude > -122.53 && longitude < -122.38;
        }

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
                        cab.cabLine.Add(new CabMetaData(cab.cabName, i++, Double.Parse(str[0]), Double.Parse(str[1]), str[2], Convert.ToInt64(str[3])));
                        UpdateBoundForDrawMap(Double.Parse(str[0]), Double.Parse(str[1]));
                    }
                }
                count += i;
                cab.cabLine.Reverse(0, cab.cabLine.Count);
                fileReader.Close();
            }
            Console.WriteLine("All cabs data have been processed. " + count);
        }
        #endregion

        //把经度转换为图像上的X坐标
        public static int LongitudeToX(double longitude)
        {
            return (int)((longitude - longitudeLowBound) * 50000);
        }
        //把纬度转换为图像上的Y坐标
        public static int LatitudeToY(double latitude)
        {
            return (int)((latitudeUpperBound - latitude) * 50000);
        }

        static void Main(string[] args)
        {
            //初始化
            cabList = new List<Cab>();
            cabNameList = new List<String>();

            InitiateGridBound();
            ReadSanFranciscoCabData();
            Bitmap bmp = new Bitmap(LongitudeToX(longitudeUpperBound) + 1, LatitudeToY(latitudeLowBound) + 1);
            Graphics g = Graphics.FromImage(bmp);
            g.InterpolationMode = InterpolationMode.HighQualityBicubic;
            g.SmoothingMode = SmoothingMode.HighQuality;
            g.CompositingQuality = CompositingQuality.HighQuality;
            g.Clear(Color.White);
            //List<float> l1 = new List<float>();
            //List<float> l2 = new List<float>();
            //List<float> l3 = new List<float>();
            //List<float> l4 = new List<float>();
            foreach (Cab cab in cabList)
            {
                foreach (CabMetaData point in cab.cabLine)
                {
                    if (InBound(point.Latitude, point.Longitude))
                    {
                        int x = LongitudeToX(point.Longitude);
                        int y = LatitudeToY(point.Latitude);
                        //if (x < 5000)
                        //{
                        //    l1.Add(point.Longitude);
                        //}
                        //if (x > 6500)
                        //{
                        //    l2.Add(point.Longitude);
                        //}
                        //if (y < 1000)
                        //{
                        //    l3.Add(point.Latitude);
                        //}
                        //if (y > 2000)
                        //{
                        //    l4.Add(point.Latitude);
                        //}
                        bmp.SetPixel(x, y, Color.Red);
                    }
                }
            }
            for (int i = 1; i < bmp.Width - 1; i++)
            {
                for (int j = 1; j < bmp.Height - 1; j++)
                {
                    if (!(bmp.GetPixel(i, j).Equals(bmp.GetPixel(i + 1, j)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i, j + 1)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i + 1, j + 1)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i - 1, j)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i, j - 1)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i - 1, j - 1)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i + 1, j - 1)) || bmp.GetPixel(i, j).Equals(bmp.GetPixel(i - 1, j + 1))))
                    {
                        bmp.SetPixel(i, j, bmp.GetPixel(i + 1, j));
                    }
                }
            }
            //l1.Sort();
            //l2.Sort();
            //l3.Sort();
            //l4.Sort();
            //Console.WriteLine("小于2100：" + l1[l1.Count - 1]);
            //Console.WriteLine("大于3600：" + l2[0]);
            //Console.WriteLine("小于1800：" + l3[0]);
            //Console.WriteLine("大于3000：" + l4[l4.Count - 1]);
            StreamWriter writer = new StreamWriter("test5.bmp");
            bmp.Save(writer.BaseStream, ImageFormat.Bmp);
            writer.Close();
        }
    }
}
