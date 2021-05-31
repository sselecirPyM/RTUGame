using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGame.Collections
{
    public class FlipList<T>
    {
        public List<T> read = new List<T>();
        public List<T> write = new List<T>();

        public List<T> GetReadList()
        {
            read.Clear();
            lock (this)
            {
                var temp = read;
                read = write;
                write = temp;
                return read;
            }
        }

        public void Add(T item)
        {
            lock (this)
            {
                write.Add(item);
            }
        }
    }
}
