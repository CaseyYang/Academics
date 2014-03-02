using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace GraduationProject
{
    class DBscanClusterSet
    {
        private List<DBscanCluster> dbscanClusterList;
        public List<DBscanCluster> DbscanClusterList
        {
            get
            {
                return dbscanClusterList;
            }
            set
            {
                dbscanClusterList = value;
            }
        }
        private List<int> clusterIdList;
        public List<int> ClusterIdList
        {
            get
            {
                return clusterIdList;
            }
        }

        public DBscanClusterSet()
        {
            dbscanClusterList = new List<DBscanCluster>();
            clusterIdList = new List<int>();
        }
        public DBscanClusterSet(List<DBscanCluster> dbscanClusterList)
        {
            this.dbscanClusterList = dbscanClusterList;
            clusterIdList = new List<int>(dbscanClusterList.Count);
            foreach (DBscanCluster cluster in dbscanClusterList)
            {
                clusterIdList.Add(cluster.ClusterId);
            }
        }
        public void Add(DBscanCluster dBscanCluster)
        {
            clusterIdList.Add(dBscanCluster.ClusterId);
            dbscanClusterList.Add(dBscanCluster);
        }
        public void Remove(DBscanCluster dBscanCluster)
        {
            clusterIdList.Remove(dBscanCluster.ClusterId);
            dbscanClusterList.Remove(dBscanCluster);
        }
    }
}