import { mac_6char, writeFileCreateDirLazy } from "@klaus-liebler/espidf-vite/utils";
import path from "node:path";
import fs from "node:fs";
import * as os from "node:os"
import * as P from "@klaus-liebler/espidf-vite/paths";
import { Context } from "@klaus-liebler/espidf-vite/context";
import { IPackageJson } from "@klaus-liebler/espidf-vite/package_json";
import { execSync } from 'node:child_process';

/*
Wir brauchen noch:
- die richige HAL-Implementierung
- die richtige Busmaster-Konfiguration (also insbesondere welche I2C-PortExtender angeschlossen sind und wie die konfiguriert sind)
  -->Auslagern in eine Funktion, die einen Pointer auf std::vector<AbstractBusmaster*> zurückgibt
- die richtige host-Konfiguration, also welche Hosts laufen sollen
   --> Auslagern in eine Funktion, die einen Pointer auf std::vector<sensact::iHost*> zurückgibt
*/

const SENSACT_JSON_FILENAME = "sensact.json"

export class Sensact {
  private pa: P.Paths
  constructor(private readonly c: Context, private readonly sensactComponentGeneratedDirectory: string) {
    this.pa = new P.Paths(c);
  }


  public prepare_sensact_files() {


    //jede Node kennt alle ApplicationIds und CommandTypes - das ist also nicht board-spezifisch, deshalb aus dem "common"-Ordner kopieren
    var content = ""
    content = fs.readFileSync(path.join(this.pa.P_FLATBUFFERS_TEMPLATES, "applicationIds.template.fbs")).toString()

    writeFileCreateDirLazy(
      path.join(this.pa.GENERATED_FLATBUFFERS_FBS, "applicationIds.fbs"),
      content.replace("//TEMPLATE_HERE",
        fs.readFileSync(path.join(this.sensactComponentGeneratedDirectory, "common", "applicationIds.fbs.inc")).toString()
      )
    );

    content = fs.readFileSync(path.join(this.pa.P_FLATBUFFERS_TEMPLATES, "commandTypes.template.fbs")).toString()
    writeFileCreateDirLazy(
      path.join(this.pa.GENERATED_FLATBUFFERS_FBS, "commandTypes.fbs"),
      content.replace("//TEMPLATE_HERE",
        fs.readFileSync(path.join(this.sensactComponentGeneratedDirectory, "common", "commandTypes.fbs.inc")).toString()
      )
    );

    //Damit die Applicationen ihre commands versenden können, werden hier passende Funktionen zum Versenden erzeugt
    content = fs.readFileSync(path.join(this.pa.P_WEB, "templates", "sendCommandImplementation.template.ts")).toString();
    writeFileCreateDirLazy(
      path.join(this.pa.GENERATED_SENSACT_TS, "sendCommandImplementation.ts"),
      content.replace("//TEMPLATE_HERE",
        fs.readFileSync(path.join(this.sensactComponentGeneratedDirectory, "common", "sendCommandImplementation.ts.inc")).toString())
    );

    //Alle im Sensact-System bekannten Apps erhalten mit diesem Code einen digitalen Zwilling in der Web-UI
    content = fs.readFileSync(path.join(this.pa.P_WEB, "templates", "sensactapps.template.ts")).toString();
    writeFileCreateDirLazy(
      path.join(this.pa.GENERATED_SENSACT_TS, "sensactapps.ts"),
      content.replace("//TEMPLATE_HERE",
        fs.readFileSync(path.join(this.sensactComponentGeneratedDirectory, "common", "sensactapps.ts.inc")).toString()
      )
    );

    //writeFileCreateDirLazy(path.join(this.pa.GENERATED_NVS_TS, "index.ts"), `export * from "./sendCommandImplementation"; export * from "./sensactapps"`);
    const pj: IPackageJson = {
      name: "@generated/sensact",
      version: "0.0.1",
      description: "Generated during Build",
      //main: "index.ts",
      author: "Generated",
      license: "No License",
      dependencies: {
        "@generated/flatbuffers_ts": "file:../flatbuffers_ts",
        "@klaus-liebler/sensact-base": "file:../../npm-packages/@klaus-liebler/sensact-base",
        "@klaus-liebler/sensact": "file:../../npm-packages/@klaus-liebler/sensact",
        "@klaus-liebler/web-components": "file:../../npm-packages/@klaus-liebler/web-components"
      }
    }
    writeFileCreateDirLazy(path.join(this.pa.GENERATED_SENSACT_TS, "package.json"), JSON.stringify(pj));
    const filterStdOut = (l: string) => true;
    const cmd = `npm install`
    console.info(`Executing ${cmd}`)
    const stdout = execSync(cmd, {
      cwd: this.pa.GENERATED_SENSACT_TS,
      env: process.env
    });
    if (stdout)
      stdout.toString().split(os.EOL).filter((v) => filterStdOut(v)).forEach(v => console.log(v.toString()))

    return;
    //Jetzt wird es spezifisch -->ES GIBT HIER NICHTS "SPEZIFISCHES"!

    if (this.pa.existsBoardSpecificPath(SENSACT_JSON_FILENAME)) {
      throw Error(`No sensact.json file found for mac 0x${mac_6char(this.c.b.mac)}.`);
    }
    const sensact_json = JSON.parse(fs.readFileSync(this.pa.boardSpecificPath("sensact.json")).toString())

    const node_id = sensact_json ? sensact_json["node_id"] : null;
    if (!node_id) {
      throw Error(`sensact.json file found for mac 0x${mac_6char(this.c.b.mac)} did not contain a node_id.`);
    }
  }
}