using System;
using System.Collections.Generic;

namespace TrajectoriesToMap
{
    /// <summary>
    /// 表示一个出租车轨迹数据点的类
    /// </summary>
    public class CabMetaData//:IEquatable<CabMetaData>
    {
        /// <summary>
        /// 出租车名称
        /// </summary>
        public String CabName { get; private set; }
        /// <summary>
        /// 每个数据点唯一标识；命名规则为：出租车名称+序列号
        /// </summary>
        public String CabMetaDataId { get; private set; }
        /// <summary>
        /// 纬度
        /// </summary>
        public double Latitude { get; private set; }
        /// <summary>
        /// 经度
        /// </summary>
        public double Longitude { get; private set; }
        /// <summary>
        /// 是否载客
        /// </summary>
        public String Fare { get; private set; }
        /// <summary>
        /// 时间戳
        /// </summary>
        public long TimeStamp { get; private set; }
        /// <summary>
        /// 扩展字段
        /// </summary>
        public String Extension;
        /// <summary>
        /// 所属cluster的ClusterId
        /// </summary>
        public int ClusterId;

        /// <summary>
        /// 默认构造函数
        /// </summary>
        public CabMetaData()
        {
        }
        /// <summary>
        /// 指定成员变量的构造函数
        /// </summary>
        /// <param name="cabName">出租车名称</param>
        /// <param name="id">每个数据点唯一标识；命名规则为：出租车名称+序列号</param>
        /// <param name="latitude">纬度</param>
        /// <param name="longitude">经度</param>
        /// <param name="fare">是否载客</param>
        /// <param name="timeStamp">时间戳</param>
        public CabMetaData(String cabName, int id, double latitude, double longitude, String fare, long timeStamp)
        {
            CabName = cabName;
            this.CabMetaDataId = cabName + id.ToString();
            this.Longitude = longitude;
            this.Latitude = latitude;
            this.Fare = fare;
            this.TimeStamp = timeStamp;
            this.Extension = "";
            this.ClusterId = -1;
        }
        /// <summary>
        /// 复制构造函数
        /// </summary>
        /// <param name="cabMetaData">要复制的CabMetaData对象实例</param>
        public CabMetaData(CabMetaData cabMetaData)
        {
            CabName = cabMetaData.CabName;
            CabMetaDataId = cabMetaData.CabMetaDataId;
            Longitude = cabMetaData.Longitude;
            Latitude = cabMetaData.Latitude;
            Fare = cabMetaData.Fare;
            TimeStamp = cabMetaData.TimeStamp;
            Extension = "";
            ClusterId = cabMetaData.ClusterId;
        }
    }
    public class CabMetaDataComparer : IEqualityComparer<CabMetaData>
    {
        public bool Equals(CabMetaData c1, CabMetaData c2)
        {
            if (c1.CabMetaDataId == c2.CabMetaDataId)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        public int GetHashCode(CabMetaData c)
        {
            return c.CabMetaDataId.GetHashCode();
        }
    }
}
