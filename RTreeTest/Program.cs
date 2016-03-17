using RTree;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RTreeTest
{
    class Program
    {
        struct Node
        {
            public float x;
            public float y;
            public float z;
        }
        static void Main(string[] args)
        {
            RTree<Node> tree = new RTree<Node>(2, 4);
            Node node1 = new Node() { x = 1, y = 1 };
            Rectangle reactangle1 = new Rectangle(node1.x, node1.y, node1.x, node1.y, 0, 0);
            tree.Add(reactangle1, node1);
            Node node2 = new Node() { x = 4, y = 3 };
            Rectangle reactangle2 = new Rectangle(node2.x, node2.y, node2.x, node2.y, 0, 0);
            tree.Add(reactangle2, node2);
            Node node3 = new Node() { x = 2, y = 9 };
            Rectangle reactangle3 = new Rectangle(node3.x, node3.y, node3.x, node3.y, 0, 0);
            tree.Add(reactangle3, node3);
            Node node4 = new Node() { x = 5, y = 7 };
            Rectangle reactangle4 = new Rectangle(node4.x, node4.y, node4.x, node4.y, 0, 0);
            tree.Add(reactangle4, node4);
            Node node5 = new Node() { x = 6, y = 10 };
            Rectangle reactangle5 = new Rectangle(node5.x, node5.y, node5.x, node5.y, 0, 0);
            tree.Add(reactangle5, node5);
            Node node6 = new Node() { x = 7, y = 6 };
            Rectangle reactangle6 = new Rectangle(node6.x, node6.y, node6.x, node6.y, 0, 0);
            tree.Add(reactangle6, node6);
            Node node7 = new Node() { x = 9, y = 11 };
            Rectangle reactangle7 = new Rectangle(node7.x, node7.y, node7.x, node7.y, 0, 0);
            tree.Add(reactangle7, node7);
            Node node8 = new Node() { x = 6, y = 2 };
            Rectangle reactangle8 = new Rectangle(node8.x, node8.y, node8.x, node8.y, 0, 0);
            Console.WriteLine("测试开始：");
            Console.WriteLine("测试Contains函数：");
            Console.Write("判断是否存在区域<2,9>：");
            List<Node> resultOfContains1 = tree.Contains(reactangle3);
            foreach (Node node in resultOfContains1)
            {
                Console.Write("<" + node.x + "," + node.y + "> ");
            }
            Console.WriteLine();
            Console.Write("判断是否存在区域8：");
            List<Node> resultOfContains2 = tree.Contains(reactangle8);
            foreach (Node node in resultOfContains2)
            {
                Console.Write("<" + node.x + "," + node.y + "> ");
            }
            Console.WriteLine();
            Console.WriteLine("测试Count值：" + tree.Count);
            Console.WriteLine("测试getBounds函数：" + tree.getBounds().ToString());
            Console.Write("测试Intersect函数：");
            List<Node> resultOfIntersect = tree.Intersects(new Rectangle((float)4.5, 5, 7, 12, 0, 0));
            foreach (Node node in resultOfIntersect)
            {
                Console.Write("<" + node.x + "," + node.y + "> ");
            }
            Console.WriteLine();
            Console.Write("测试Nearest函数：");
            List<Node> resultOfNearset = tree.Nearest(new Point(2, 4, 0), 1000);
            foreach (Node node in resultOfNearset)
            {
                Console.Write("<" + node.x + "," + node.y + "> ");
            }
            Console.WriteLine();
            Console.WriteLine("测试GetMBRs函数：");
            List<Rectangle> rectangleList = tree.GetMBRs();
            foreach (Rectangle rectangle in rectangleList)
            {
                Console.WriteLine("MBR[" + rectangle.ToString() + "], ");
            }
            Console.WriteLine("测试结束！");
        }
    }
}
