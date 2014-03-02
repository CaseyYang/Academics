using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace GraduationProject
{
    class ThreadManager
    {
        public ManualResetEvent eventX;
        public static int iCount = 0;
        public static int iMaxCount = 0;
        public ThreadManager(int count, ManualResetEvent eventX)
        {
            this.eventX = eventX;
            iMaxCount = count;
        }
        public void ThreadFunc1(Object obj)
        {
            
        }
        public void ThreadFunc2(Object obj)
        {

        }
    }

    class DataPackage
    {

    }
}
