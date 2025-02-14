import { mac_6char, writeFileCreateDirLazy } from "@klaus-liebler/espidf-vite-secure-build-tools/utils";
import path from "node:path";
import fs from "node:fs";
import * as P from "@klaus-liebler/espidf-vite-secure-build-tools/paths";
import { Context } from "@klaus-liebler/espidf-vite-secure-build-tools/context";
import * as npm from "@klaus-liebler/espidf-vite-secure-build-tools/npm"


const SENSACT_JSON_FILENAME = "sensact.json"

export class Sensact {

  AddSensactNodeAndModelDescriptorToDefines(defines: any) {
    const dp=path.join(this.sensactComponentGeneratedDirectory, this.GetNodeId(), "node_descriptor.json")
    if (!fs.existsSync(dp)) {
      throw Error(`No node_descriptor.json file found for mac 0x${mac_6char(this.c.b.mac)} at ${dp}.`);
    }
    const sensact_json = JSON.parse(fs.readFileSync(dp, "utf8"))
    return {...defines, ...sensact_json};
  }


  private pa: P.Paths
  constructor(private readonly c: Context, private readonly sensactComponentGeneratedDirectory: string) {
    this.pa = new P.Paths(c);
  }

  private  templateHere(templatePath:string, incPath:string, destPath:string) {
    var content = fs.readFileSync(templatePath).toString()
    writeFileCreateDirLazy(
      destPath,
      content.replace("//TEMPLATE_HERE",
        fs.readFileSync(incPath).toString()
      )
    );
  }

  public GetNodeId():string{
    const sjp=this.pa.boardSpecificPath(SENSACT_JSON_FILENAME)
    if (!fs.existsSync(sjp)) {
      throw Error(`No sensact.json file found for mac 0x${mac_6char(this.c.b.mac)} at ${sjp}.`);
    }
    const sensact_json = JSON.parse(fs.readFileSync(sjp).toString())

    const node_id = sensact_json ? sensact_json["node_id"] : null;
    if (!node_id) {
      throw Error(`sensact.json file found for mac 0x${mac_6char(this.c.b.mac)} did not contain a node_id.`);
    }
    return node_id;
  }


  public PrepareSensactFiles() {


    //jede Node kennt alle ApplicationIds und CommandTypes - das ist also nicht board-spezifisch, deshalb aus dem "common"-Ordner kopieren
    this.templateHere(
      path.join(this.pa.P_FLATBUFFERS_TEMPLATES, "applicationIds.template.fbs"),
      path.join(this.sensactComponentGeneratedDirectory, "common", "applicationIds.fbs.inc"),
      path.join(this.pa.GENERATED_FLATBUFFERS_FBS, "applicationIds.fbs")
    );

    this.templateHere(
      path.join(this.pa.P_FLATBUFFERS_TEMPLATES, "commandTypes.template.fbs"),
      path.join(this.sensactComponentGeneratedDirectory, "common", "commandTypes.fbs.inc"),
      path.join(this.pa.GENERATED_FLATBUFFERS_FBS, "commandTypes.fbs")
    );


    //Damit die Applicationen ihre commands versenden können, werden hier passende Funktionen zum Versenden erzeugt
    //Zunächst ein Projekt für die Sende
    var project ="sensact_sendCommandImplementation"
    this.templateHere(
      path.join(this.pa.P_WEB, "templates", "sendCommandImplementation.template.ts"),
      path.join(this.sensactComponentGeneratedDirectory, "common", "sendCommandImplementation.ts.inc"),
      path.join(this.c.c.generatedDirectory, project, "sendCommandImplementation.ts")
    );
    
    npm.CreateAndInstallNpmProjectLazily(
      path.join(this.c.c.generatedDirectory, project),
      {
        name: `@generated/${project}`,
        version: "0.0.1",
        description: "Generated during Build",
        author: "Generated",
        license: "No License",
        dependencies: {
          "@generated/flatbuffers_ts": "file:../flatbuffers_ts",
          "@klaus-liebler/sensact-base":"file:../../npm-packages/@klaus-liebler/sensact-base"
        }
      }
    );


    //Alle im Sensact-System bekannten Apps erhalten mit diesem Code einen digitalen Zwilling in der Web-UI
    project ="sensact_appsbuilder"
    this.templateHere(
      path.join(this.pa.P_WEB, "templates", "sensactapps.template.ts"),
      path.join(this.sensactComponentGeneratedDirectory, this.GetNodeId(), "sensactapps_local.ts.inc"),
      path.join(this.c.c.generatedDirectory, project, "sensactapps.ts")
    );
    npm.CreateAndInstallNpmProjectLazily(
      path.join(this.c.c.generatedDirectory, project),
      {
        name: `@generated/${project}`,
        version: "0.0.1",
        description: "Generated during Build",
        author: "Generated",
        license: "No License",
        dependencies: {
          "@generated/flatbuffers_ts": "file:../flatbuffers_ts",
          "@klaus-liebler/sensact-base":"file:../../npm-packages/@klaus-liebler/sensact-base",
          "@klaus-liebler/web-components-sensact":"file:../../npm-packages/@klaus-liebler/web-components-sensact"
        }
      }
    );
  }
}