using System;
using System.Text;

namespace RTree
{
    /// <summary>
    /// 表示一个处于三维空间内的矩形
    /// </summary>
    public class Rectangle
    {
        /// <summary>
        /// 空间维数，暂且定为三维
        /// </summary>
        internal const int DIMENSIONS = 3;

        /// <summary>
        /// 表示该矩形在每个维度上的上界
        /// </summary>
        private float[] max;
        /// <summary>
        /// 返回或设置矩形框的东北角顶点（即各个维度的上界）
        /// </summary>
        public float[] Max
        {
            get { return max; }
            set { max = value; }
        }

        /// <summary>
        /// 表示该矩形在每个维度上的下界
        /// </summary>
        private float[] min;
        /// <summary>
        /// 返回或设置矩形框的西南角顶点（即各个维度的下界）
        /// </summary>
        public float[] Min
        {
            get { return min; }
            set { min = value; }
        }

        /// <summary>
        /// 矩形构造函数
        /// </summary>
        /// <param name="x1">一个顶点的X坐标</param>
        /// <param name="y1">一个顶点的Y坐标</param>
        /// <param name="x2">对角线处顶点的X坐标</param>
        /// <param name="y2">对角线处顶点的Y坐标</param>
        /// <param name="z1">一个顶点的Z坐标</param>
        /// <param name="z2">对角线处顶点的Z坐标</param>
        public Rectangle(float x1, float y1, float x2, float y2, float z1, float z2)
        {
            min = new float[DIMENSIONS];
            max = new float[DIMENSIONS];
            set(x1, y1, x2, y2, z1, z2);
        }

        /// <summary>
        /// 矩形构造函数
        /// </summary>
        /// <param name="min">该矩形在每个维度上的下界</param>
        /// <param name="max">该矩形在每个维度上的上界</param>
        public Rectangle(float[] min, float[] max)
        {
            if (min.Length != DIMENSIONS || max.Length != DIMENSIONS)
            {
                throw new Exception("Error in Rectangle constructor: " +
                          "min and max arrays must be of length " + DIMENSIONS);
            }

            this.min = new float[DIMENSIONS];
            this.max = new float[DIMENSIONS];

            set(min, max);
        }

        /// <summary>
        /// 确定矩形在各个维度上的上下界
        /// </summary>
        /// <param name="x1">一个顶点的X坐标</param>
        /// <param name="y1">一个顶点的Y坐标</param>
        /// <param name="x2">对角线处顶点的X坐标</param>
        /// <param name="y2">对角线处顶点的Y坐标</param>
        /// <param name="z1">一个顶点的Z坐标</param>
        /// <param name="z2">对角线处顶点的Z坐标</param>
        internal void set(float x1, float y1, float x2, float y2, float z1, float z2)
        {
            min[0] = Math.Min(x1, x2);
            min[1] = Math.Min(y1, y2);
            min[2] = Math.Min(z1, z2);
            max[0] = Math.Max(x1, x2);
            max[1] = Math.Max(y1, y2);
            max[2] = Math.Max(z1, z2);
        }

        /// <summary>
        /// 确定矩形在各个维度上的上下界
        /// </summary>
        /// <param name="min">该矩形在每个维度上的下界</param>
        /// <param name="max">该矩形在每个维度上的上界</param>
        internal void set(float[] min, float[] max)
        {
            System.Array.Copy(min, 0, this.min, 0, DIMENSIONS);
            System.Array.Copy(max, 0, this.max, 0, DIMENSIONS);
        }

        /// <summary>
        /// 拷贝函数
        /// </summary>
        /// <returns>返回返回该矩形的一个新的实例</returns>
        internal Rectangle copy()
        {
            return new Rectangle(min, max);
        }

        /// <summary>
        /// 判断该矩形的上下界是否与矩形r的上下界相同
        /// </summary>
        /// <param name="r">待比较的矩形</param>
        /// <returns>返回若任一维的上界或下界相同，返回true；否则返回false</returns>
        internal bool edgeOverlaps(Rectangle r)
        {
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (min[i] == r.min[i] || max[i] == r.max[i])
                {
                    return true;
                }
            }
            return false;
        }

        /// <summary>
        /// 判断该矩形与矩形r是否存在重叠
        /// </summary>
        /// <param name="r">待比较的矩形</param>
        /// <returns>返回若存在重叠，返回true；否则返回false</returns>
        internal bool intersects(Rectangle r)
        {
            // Every dimension must intersect. If any dimension
            // does not intersect, return false immediately.
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (max[i] < r.min[i] || min[i] > r.max[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 判断该矩形是否完全包含矩形r
        /// </summary>
        /// <param name="r">待比较的矩形</param>
        /// <returns>返回若完全包含，返回true；否则返回false</returns>
        internal bool contains(Rectangle r)
        {
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (max[i] < r.max[i] || min[i] > r.min[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 判断该矩形是否被矩形r完全包含
        /// </summary>
        /// <param name="r">待比较的矩形</param>
        /// <returns>返回若被完全包含，返回true；否则返回false</returns>
        internal bool containedBy(Rectangle r)
        {
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (max[i] > r.max[i] || min[i] < r.min[i])
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 计算一点与该矩形间的距离。若点在矩形内，则距离为0
        /// </summary>
        /// <param name="p">待计算的点</param>
        /// <returns>返回点到矩形各顶点间距离的最小值</returns>
        internal float distance(Point p)
        {
            float distanceSquared = 0;
            for (int i = 0; i < DIMENSIONS; i++)
            {
                float greatestMin = Math.Max(min[i], p.coordinates[i]);
                float leastMax = Math.Min(max[i], p.coordinates[i]);
                if (greatestMin > leastMax)
                {
                    distanceSquared += ((greatestMin - leastMax) * (greatestMin - leastMax));
                }
            }
            return (float)Math.Sqrt(distanceSquared);
        }

        /// <summary>
        /// 计算两矩形间的距离。若两矩形存在重叠，则距离为0
        /// </summary>
        /// <param name="r">待计算的矩形</param>
        /// <returns>返回两矩形间的距离</returns>
        internal float distance(Rectangle r)
        {
            float distanceSquared = 0;
            for (int i = 0; i < DIMENSIONS; i++)
            {
                float greatestMin = Math.Max(min[i], r.min[i]);
                float leastMax = Math.Min(max[i], r.max[i]);
                if (greatestMin > leastMax)
                {
                    distanceSquared += ((greatestMin - leastMax) * (greatestMin - leastMax));
                }
            }
            return (float)Math.Sqrt(distanceSquared);
        }

        /// <summary>
        /// 计算一个点到矩形在某个维度的边界的垂直距离
        /// </summary>
        /// <param name="dimension">指定维度</param>
        /// <param name="point">点在指定维度上的坐标</param>
        /// <returns>返回点到矩形在该维度的边界的垂直距离</returns>
        internal float distanceSquared(int dimension, float point)
        {
            float distanceSquared = 0;
            float tempDistance = point - max[dimension];
            for (int i = 0; i < 2; i++)
            {
                if (tempDistance > 0)
                {
                    distanceSquared = (tempDistance * tempDistance);
                    break;
                }
                tempDistance = min[dimension] - point;
            }
            return distanceSquared;
        }

        /// <summary>
        /// 计算两矩形间的最大距离
        /// </summary>
        /// <param name="r">待计算矩形</param>
        /// <returns>返回两矩形间的最大距离</returns>
        internal float furthestDistance(Rectangle r)
        {
            float distanceSquared = 0;
            for (int i = 0; i < DIMENSIONS; i++)
            {
                distanceSquared += Math.Max(r.min[i], r.max[i]);
            }
            return (float)Math.Sqrt(distanceSquared);
        }

        /// <summary>
        /// 计算若把矩形r加入现有矩形后，需要增加的面积
        /// </summary>
        /// <param name="r">待计算的矩形</param>
        /// <returns>返回若把矩形r加入现有矩形后，需要增加的面积</returns>
        internal float enlargement(Rectangle r)
        {
            float enlargedArea = (Math.Max(max[0], r.max[0]) - Math.Min(min[0], r.min[0])) *
                                 (Math.Max(max[1], r.max[1]) - Math.Min(min[1], r.min[1]));

            return enlargedArea - area();
        }

        /// <summary>
        /// 计算矩形面积
        /// </summary>
        /// <returns>返回矩形面积</returns>
        internal float area()
        {
            return (max[0] - min[0]) * (max[1] - min[1]);
        }

        /// <summary>
        /// 把矩形r加入到现有矩形中
        /// </summary>
        /// <param name="r">要加入的矩形</param>
        internal void add(Rectangle r)
        {
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (r.min[i] < min[i])
                {
                    min[i] = r.min[i];
                }
                if (r.max[i] > max[i])
                {
                    max[i] = r.max[i];
                }
            }
        }

        /// <summary>
        /// 得到一个包含该矩形和矩形r的新矩形实例；不会改变原矩形实例
        /// </summary>
        /// <param name="r">要加入的矩形</param>
        /// <returns></returns>
        internal Rectangle union(Rectangle r)
        {
            Rectangle union = this.copy();
            union.add(r);
            return union;
        }

        /// <summary>
        /// 辅助函数：比较两个float型数组是否包含相同的元素
        /// </summary>
        /// <param name="a1">待比较的float型数组</param>
        /// <param name="a2">待比较的float型数组</param>
        /// <returns>若两个数组包含相同元素，返回true；否则返回false</returns>
        internal bool CompareArrays(float[] a1, float[] a2)
        {
            if ((a1 == null) || (a2 == null))
                return false;
            if (a1.Length != a2.Length)
                return false;

            for (int i = 0; i < a1.Length; i++)
                if (a1[i] != a2[i])
                    return false;
            return true;
        }

        /// <summary>
        /// 判断该矩形是否和另一个对象是同一个矩形
        /// </summary>
        /// <param name="obj">待判断的对象</param>
        /// <returns>若两者在各个维度上的上下界相同，则返回true；否则返回false</returns>
        public override bool Equals(object obj)
        {
            bool equals = false;
            if (obj.GetType() == typeof(Rectangle))
            {
                Rectangle r = (Rectangle)obj;
#warning possible didn't convert properly
                if (CompareArrays(r.min, min) && CompareArrays(r.max, max))
                {
                    equals = true;
                }
            }
            return equals;
        }

        /// <summary>
        /// 判断该矩形实例是否和另一个对象指向同一个实例
        /// </summary>
        /// <param name="o">待判断的对象</param>
        /// <returns>若两者指向同一个实例，则返回true；否则返回false</returns>
        internal bool sameObject(object o)
        {
            return base.Equals(o);
        }

        /// <summary>
        /// 得到该矩形的一个字符串表达，形如(1.2, 3.4), (5.6, 7.8)
        /// </summary>
        /// <returns>返回该矩形的一个字符串表达</returns>
        public override string ToString()
        {
            StringBuilder sb = new StringBuilder();
            // min coordinates
            sb.Append('(');
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (i > 0)
                {
                    sb.Append(", ");
                }
                sb.Append(min[i]);
            }
            sb.Append("), (");
            // max coordinates
            for (int i = 0; i < DIMENSIONS; i++)
            {
                if (i > 0)
                {
                    sb.Append(", ");
                }
                sb.Append(max[i]);
            }
            sb.Append(')');
            return sb.ToString();
        }
    }
}
