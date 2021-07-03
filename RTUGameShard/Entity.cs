using System;
using System.Collections.Generic;
using System.Text;

namespace RTUGame.Game
{
    public class Entity
    {
        public long id;
        public string type;
        public Transform transform;
        public string model;
        public float health;
        public float healthMax;
        public float shield;
        public float shieldMax;
        public float speed;
        public float speedBase;

    }
}
