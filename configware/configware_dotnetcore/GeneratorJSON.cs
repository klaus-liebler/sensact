using System.Text.Json;
using System.Text.Json.Serialization;
using System.Collections.Generic;

namespace Klli.Sensact.Config{
    public class JSONAppDescription{

        public JSONAppDescription(ushort AppId, string AppName, string AppType){
            this.AppId=AppId;
            this.AppName=AppName;
            this.AppType=AppType;
        }
        public ushort AppId{get;}
        public string AppName{get;}
        public string AppType{get;}
    }

    public class JSONAppDescriptionOneTarget:JSONAppDescription{
        public JSONAppDescriptionOneTarget(ushort AppId, string AppName, string AppType, ushort TargetAppId):base(AppId, AppName, AppType){
            this.TargetAppId=TargetAppId;
        }
        public ushort TargetAppId{get;}
    }

    public class JSONAppDescriptionMultipleTarget:JSONAppDescription{
        public JSONAppDescriptionMultipleTarget(ushort AppId, string AppName, string AppType, ICollection<ushort> TargetAppIds):base(AppId, AppName, AppType){
            this.TargetAppIds=new List<ushort>(TargetAppIds);
        }
        public List<ushort> TargetAppIds{get;}
    }
}