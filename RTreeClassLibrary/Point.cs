namespace RTree
{
    /// <summary>
    /// 表示一个处于三维空间内的点
    /// </summary>
    public class Point
    {
        /// <summary>
        /// 空间维数，暂且定为三维
        /// </summary>
        private const int DIMENSIONS = 3;
        /// <summary>
        /// 点坐标
        /// </summary>
        internal float[] coordinates;
        /// <summary>
        /// 点构造函数
        /// </summary>
        /// <param name="x">点的X坐标</param>
        /// <param name="y">点的Y坐标</param>
        /// <param name="z">点的Z坐标</param>
        public Point(float x, float y, float z)
        {
            coordinates = new float[DIMENSIONS];
            coordinates[0] = x;
            coordinates[1] = y;
            coordinates[2] = z;
        }
    }
}
