﻿// ------------------------------------------------------------------------------
// <auto-generated>
//     Dieser Code wurde von einem Tool generiert.
//     Laufzeitversion: 14.0.0.0
//  
//     Änderungen an dieser Datei können fehlerhaftes Verhalten verursachen und gehen verloren, wenn
//     der Code neu generiert wird.
// </auto-generated>
// ------------------------------------------------------------------------------
namespace Klli.Sensact.Config.Templates
{
    using System.Linq;
    using System.Text;
    using System.Collections.Generic;
    using System;
    
    /// <summary>
    /// Class to produce the template output
    /// </summary>
    
    #line 1 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
    [global::System.CodeDom.Compiler.GeneratedCodeAttribute("Microsoft.VisualStudio.TextTemplating", "14.0.0.0")]
    public partial class HC_MODEL_CPP : CommonFileHeader
    {
#line hidden
        /// <summary>
        /// Create the template output
        /// </summary>
        public override string TransformText()
        {
            
            #line 6 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
 RenderHeader("hc_model.cpp", "autogen", version, "The Home Model", "All params and details about the home model"); 
            
            #line default
            #line hidden
            this.Write(@"
#include ""common.h""
#include ""cModel.h""
#include ""cBlind.h""
#include ""cRotar.h""
#include ""cPoweritem.h""
#include ""cPushbutton.h""
#include ""cPwm.h""
#include ""cStandbycontroller.h""
#include ""cBrightnessSensor.h""
#include ""cBell.h""
#include ""cRgbw.h""
#include <chrono>
#include ""date.h""
#include ""bh1750.h""

using namespace std;

namespace sensact {

const char *MODEL::ApplicationNames[]={
		");
            
            #line 28 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(ApplicationNames));
            
            #line default
            #line hidden
            this.Write("\r\n\t};\r\n\r\n\r\n");
            
            #line 32 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"

       foreach (ModelCppItem i in Items)
       {

            
            #line default
            #line hidden
            this.Write("#ifdef NODE_");
            
            #line 36 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i.NodeId));
            
            #line default
            #line hidden
            this.Write("\r\n\r\neNodeID NODE = eNodeID::");
            
            #line 38 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i.NodeId));
            
            #line default
            #line hidden
            this.Write(";\r\n\r\nconst char MODEL::ModelString[] =\"");
            
            #line 40 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i.ModelInfo));
            
            #line default
            #line hidden
            this.Write("\";\r\n\r\n#include <cModel_base.inc>\r\n\r\n\r\n");
            
            #line 45 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i.StaticApplicationInitializers));
            
            #line default
            #line hidden
            this.Write("\r\n\r\n\r\n\r\ncApplication *MODEL::Glo2locCmd[] = {\r\n");
            
            #line 50 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"
            this.Write(this.ToStringHelper.ToStringWithCulture(i.Glo2LocPointers));
            
            #line default
            #line hidden
            this.Write("\r\n};\r\n\r\n\r\n\r\ncApplication *MODEL::Glo2locEvt[] = { };\r\n#endif\r\n\r\n");
            
            #line 58 "C:\sensact\configware\Klli.Sensact.Config\Templates\HC_MODEL_CPP.tt"

           
       }

            
            #line default
            #line hidden
            this.Write("\r\n}\r\n\r\n");
            return this.GenerationEnvironment.ToString();
        }
    }
    
    #line default
    #line hidden
}