using System.Collections.Generic;
using System.Text;

namespace Klli.Sensact.Config.Templates
{

    
    partial class APPLICATION_H
    {
        public string CommandHeaders;
        public string version;
    }

    partial class APPLICATION_CPP
    {
        public string CommandParseImplementation;
        public string CommandEmptyHandlerImplementation=string.Empty;
        public string CommandCreateImplementation = string.Empty;

        public string version;
    }
    partial class HC_APPIDS_H
    {
        public List<string> AppIds=new List<string>();
        public List<string> Nodes=new List<string>();
        public string version;
    }

    partial class HC_MODEL_CPP
    {
        public List<ModelCppItem> Items = new List<ModelCppItem>();
        public string version;
        public string ModelReference;
        public string[] ApplicationNames;

    }

    public class ModelCppItem
    {
        public string NodeId;
        public StringBuilder StaticApplicationInitializers = new StringBuilder();
        public StringBuilder Glo2LocPointers = new StringBuilder();
        public StringBuilder Glo2LocEvents = new StringBuilder();
        public string version;
        public string ModelInfo;
    }

    partial class HC_COMMANDANDEVENTTYPES_H
    {
        public string version;
    }

    partial class LinkerScript
    {
        public string Target;
        public uint FlashInK;
        public uint RamInK;
        public uint CCRamInK = 64;
        public uint FlashOrigin = 0x08000000;
        public uint RamOrigin = 0x20000000;
        public uint CCRamOrigin = 0x10000000;
        public uint MinStackSize = 0x200;
        public uint MinHeapSize = 0;
    }

}
