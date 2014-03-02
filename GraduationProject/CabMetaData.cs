using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    public class CabMetaData//:IEquatable<CabMetaData>
    {
        private String cabName;                  //出租车名称
        public String CabName
        {
            get { return cabName; }
        }
        private String cabMetaDataId;            //每个数据点唯一标识；命名规则为：出租车名称+序列号
        public String CabMetaDataId
        {
            get { return cabMetaDataId; }
        }
        private double latitude;                 //纬度
        public double Latitude
        {
            get { return latitude; }
        }
        private double longitude;                //经度
        public double Longitude
        {
            get { return longitude; }
        }
        private String fare;                     //是否载客
        public String Fare
        {
            get { return fare; }
        }
        private long timeStamp;                  //时间戳
        public long TimeStamp
        {
            get { return timeStamp; }
        }
        public String extension;                //扩展字段
        public int clusterId;                   //所属cluster的clusterId

        public CabMetaData()
        {
        }
        public CabMetaData(String cabName, int id, double latitude, double longitude, String fare, long timeStamp)
        {
            this.cabName = cabName;
            this.cabMetaDataId = cabName + id.ToString();
            this.longitude = longitude;
            this.latitude = latitude;
            this.fare = fare;
            this.timeStamp = timeStamp;
            this.extension = "";
            this.clusterId = -1;
        }
        public CabMetaData(CabMetaData cabMetaData)
        {
            this.cabName = cabMetaData.cabName;
            this.cabMetaDataId = cabMetaData.cabMetaDataId;
            this.longitude = cabMetaData.longitude;
            this.latitude = cabMetaData.latitude;
            this.fare = cabMetaData.fare;
            this.timeStamp = cabMetaData.timeStamp;
            this.extension = "";
            this.clusterId = cabMetaData.clusterId;
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
