#!/usr/bin/env node
// require node version >= 10
const { resolve, dirname } = require('path');
const { readdir, readFile, writeFile, mkdir } = require('fs').promises;
const { exec } = require('shelljs');
const { program } = require('commander');

program
    .name('gen-graph')
    .description('CLI to generate include/base graph from Beacon TU.json files')
    .version('0.0.1')
    .requiredOption('-d, --json-dir <dir>', 'input json file directory')
    .requiredOption('-o, --output-dir <dir>', 'output directory')
    .option('-t, --types <types...>', 'include base')
    .option('-m, --with-merge', 'need merge json')
    .addHelpText('after', `
Example call:
    node gen-graph --json-dir ./data --output-dir ./result/graph --with-merge --types include base
    node gen-graph --help`)


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


async function genIncludeLinksFromJsonFile(filename) {
    const TUjsonStr = await readFile(filename, { encoding: 'utf-8' });
    const TU = JSON.parse(TUjsonStr);
    if (TU.includeList === undefined) {
        return []
    }
    const links = TU.includeList.map(function (includeInfo, index) {
        const caller = includeInfo.sourceLocation.file;
        const callee = includeInfo.file.name;
        return `\t"${caller}" -> "${callee}"`;
    })
    return [...new Set(links)];
}

async function genBaseLinksFromJsonFile(filename) {
    const TUjsonStr = await readFile(filename, { encoding: 'utf-8' });
    const TU = JSON.parse(TUjsonStr);
    if (TU.classList === undefined) {
        return []
    }
    const links = TU.classList.map(function (classInfo, index) {
        if (classInfo.baseList === undefined) {
            return []
        }
        return classInfo.baseList.map(function (baseInfo, index) {
            return `\t"${classInfo.name}" -> "${baseInfo.name}"`;
        });
    }).flat(1)
    return [...new Set(links)];
}


async function doMain(jsonDir, outputDir, needMerge, types) {
    const linksMap = {}
    linksMap["include"] = {}
    linksMap["base"] = {}
    for await (const filename of allFiles(jsonDir, /\.json$/)) {
        if (!filename.endsWith('.json')) { continue; }
        // const TU = await getTUFromJsonFile(filename)
        // console.log(`getTUFrom${filename} = ${TU.classList}`);
        linksMap["include"][filename] = await genIncludeLinksFromJsonFile(filename);
        linksMap["base"][filename] = await genBaseLinksFromJsonFile(filename);
    }
    const linkTypes = ["include", "base"]
    for (const linkTypeIndex in linkTypes) {
        const linkType = linkTypes[linkTypeIndex];
        if (!types.includes(linkType)) {
            continue;
        }
        if (needMerge) {
            const mergeFile = outputDir + '/merge-all';
            linksMap[linkType][mergeFile] = [...new Set(Object.values(linksMap[linkType]).flat(1))];
        }
        for (const filename in linksMap[linkType]) {
            const graph = `digraph all {\n${linksMap[linkType][filename].join('\n')}\n}`;
            const outputfile = outputDir + `/${linkType}/` + filename.substring(filename.lastIndexOf('/') + 1);
            await mkdir(dirname(outputfile), { recursive: true });
            await writeFile(`${outputfile}.${linkType}.dot`, graph)
            if (exec(`dot ${outputfile}.${linkType}.dot -T png -o ${outputfile}.${linkType}.dot.png`).code !== 0) {
                console.error(`Error: generate include graph fail for ${outputfile}`)
            } else {
                console.log(`generate include graph in ${outputfile}.${linkType}.dot.png`)
            }
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
    await doMain(options.jsonDir, options.outputDir, options.withMerge, options.types);
}

main().then()