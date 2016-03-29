using System.Collections.Generic;
using CETech.World;
using MsgPack;

namespace CETech
{
    public partial class UnitManager
    {
        private static int SpawnImpl(int world, long unit)
        {
            var unit_resource = ResourceManager.Get<Dictionary<MessagePackObject, MessagePackObject>>(
                UnitResource.Type, unit);

            var types = unit_resource["type"].AsList();
            var data = unit_resource["data"].AsList();
            var ent_list = unit_resource["ent"].AsList();
            var ent_count = unit_resource["ent_count"].AsInt32();


            var entities = new int[ent_count];
            for (int i = 0; i < ent_count; i++)
            {
                entities[i] = EntityManager.Create();
            }

            for (var i = 0; i < types.Count; ++i)
            {
                var ents = ent_list[i].AsList();
                var edata = data[i].AsList();

                var entities_id = new int[ents.Count];
                var entities_body = new MessagePackObjectDictionary[ents.Count];
                for (int j = 0; j < ents.Count; j++)
                {
                    entities_id[j] = entities[ents[j].AsInt32()];
                    entities_body[j] = edata[j].AsDictionary();
                }

                ComponentSystem.Spawn(world, types[i].AsInt64(), entities_id, entities_body);
            }

            return entities[0];
        }

        private static void InitImpl()
        {
        }

        private static void ShutdownImpl()
        {
        }
    }
}