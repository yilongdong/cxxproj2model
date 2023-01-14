#!/usr/bin/env node
// require node version >= 10
const { resolve, dirname } = require('path');
const { readdir, readFile, writeFile, mkdir } = require('fs').promises;
const { exec } = require('shelljs');
const { program } = require('commander');

program
    .name('gen-uml')
    .description('CLI to generate class graph from Beacon TU.json files')
    .version('0.0.1')
    .requiredOption('-d, --json-dir <dir>', 'input json file directory')
    .requiredOption('-o, --output-dir <dir>', 'output directory')
    .addHelpText('after', `
Example call:
    node gen-class --json-dir ./data --output-dir ./result/graph
    node gen-class --help
目前这个类图还非常不完善，只展示继承关系，其他关系目前还没写`)

class ClassNode {
    constructor(cls) {
        this.name = cls.name;
        const access_map = {
            "AC_PUBLIC": "+",
            "AC_PROTECTED": "#",
            "AC_PRIVATE": "-",
            "AC_UNKNOWN": "unknown"
        }
        if (cls.fieldList === undefined) {
            this.fields = [];
        }
        else {
            this.fields = cls.fieldList.map(function (field, field_index) {
                return `{field} ${access_map[field.access]} ${field.var.type} ${field.var.name}`;
            });
        }
        
        if (cls.methodList === undefined) {
            this.methods = [];
        }
        else {
            this.methods = cls.methodList.map(function (method, method_index) {
                let type = "{method}";
                if (method.isVirtual || method.isPureVirtual) {
                    type += " {abstract}";
                }
                if (method.isStatic) {
                    type += " {static}";
                }
                if (method.paramList === undefined) {
                    return `${type} ${access_map[method.access]} ${method.returnType} ${method.name}()`;
                }
                const params_str = method.paramList.map(function (param, index) {
                    if (param.defaultValue === undefined) {
                        return `${param.var.type} ${param.var.name}`;
                    }
                    return `${param.var.type} ${param.var.name} = ${param.defaultValue}`;
                }).join(', ');
                return `${type} ${access_map[method.access]} ${method.returnType} ${method.name}(${params_str})`;
            });
        }
    }

    to_string() {
        if (this.name == "") {
            return `class UNKNOWN {}`
        }
        return `class ${this.name} {\n\t${this.fields.join('\n\t')}\n\t${this.methods.join('\n\t')}\n}`
    }
}

class Link {
    constructor(from, to, relation_symbol) {
        this.from = from;
        this.to = to;
        this.relation_symbol = relation_symbol;
    }

    to_string() {
        return `${this.from} ${this.relation_symbol} ${this.to}`;
    }
}

async function* allFiles(dir, regex) {
    const direntries = await readdir(dir, { withFileTypes: true });
    for (const direntry of direntries) {
        const filepath = resolve(dir, direntry.name);
        if (!regex.test(filepath)) {
            continue;
        }
        if (direntry.isDirectory()) {
            yield* allFiles(filepath);
        }
        else {
            yield filepath;
        }
    }
}

function genClassUML(nodes, links) {
    const nodes_desc = nodes.map(function (node, node_index) {
        return node.to_string();
    }).join('\n');

    const links_desc = links.map(function (link, link_index) {
        return link.to_string();
    }).join('\n');

    return `@startuml\n${nodes_desc}\n${links_desc}\n@enduml`;
}

async function doMain(jsonDir, outputDir) {
    for await (const filename of allFiles(jsonDir, /\.json$/)) {
        if (!filename.endsWith('.json')) continue;
        const content = await readFile(filename, { encoding: 'utf-8' })
        const TU = JSON.parse(content)
        if (TU.classList === undefined) continue;

        const data = {}
        data["nodes"] = TU.classList.map(function (cls, cls_index) {
            return new ClassNode(cls);
        });
        data["links"] = TU.classList.map(function (cls, cls_index) {
            if (cls.baseList === undefined) return []
            return cls.baseList.map(function (base, base_index) {
                if (base.isVirtual) {
                    return new Link(cls.name, base.name, '..|>');
                }
                return new Link(cls.name, base.name, '--|>');
            });
        }).flat(1);
        const graph = genClassUML(data["nodes"], data["links"]);
        const linkType = "class";
        const outputfile = outputDir + `/${linkType}/` + filename.substring(filename.lastIndexOf('/') + 1);
        await mkdir(dirname(outputfile), { recursive: true });
        await writeFile(`${outputfile}.${linkType}.uml`, graph);
        if (exec(`plantuml ${outputfile}.${linkType}.uml -tpng`).code !== 0) {
            console.error(`Error: generate class graph fail for ${outputfile}`)
        }
        else {
            console.log(`output = ${outputfile}.${linkType}.png`);
        }
    }
    
}


async function main() {
    const options = program.opts();
    program.parse();
    if (options.jsonDir.endsWith('/')) {
        options.jsonDir = options.jsonDir.substring(0, options.jsonDir.length - 1);
    }
    if (options.outputDir.endsWith('/')) {
        options.outputDir = options.outputDir.substring(0, options.outputDir.length - 1);
    }
    await doMain(options.jsonDir, options.outputDir);
}

main().then()