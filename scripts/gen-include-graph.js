#!/usr/bin/env node
// require node version >= 10
const { resolve, dirname } = require('path');
const { readdir, readFile, writeFile, mkdir } = require('fs').promises;
const { exec } = require('shelljs')
const { program } = require('commander');

program
    .name('gen-include-graph')
    .description('CLI to generate include graph from Beacon TU.json files')
    .version('0.0.1')
    .requiredOption('-d, --json-dir <dir>', 'input json file directory')
    .requiredOption('-o, --output-dir <dir>', 'output directory')
    .option('-m, --with-merge', 'need merge json')
    
program.addHelpText('after', `
Example call:
    gen-include-graph --json-dir ./data --output-dir ./result/includeGraph --with-merge
    gen-include-graph --help`)


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

function getLinksFromIncludeList(includeList) {
    const links = includeList.map(function (includeInfo, index) {
        const caller = includeInfo.hashLocInfo.file;
        const callee = includeInfo.absolutePath;
        return `\t"${caller}" -> "${callee}"`;
    })
    return [...new Set(links)];
}

async function genLinksFromJsonFile(filename) {
    const data = await readFile(filename, { encoding: 'utf-8' });
    const TUs = JSON.parse(data);
    if (TUs.length === 0) {
        return "";
    }
    return getLinksFromIncludeList(TUs[0].include_list);
}

async function doMain(jsonDir, outputDir, needMerge) {
    if (jsonDir.endsWith('/')) {
        jsonDir = jsonDir.substring(0, jsonDir.length - 1);
    }
    if (outputDir.endsWith('/')) {
        outputDir = outputDir.substring(0, outputDir.length - 1);
    }
    const linksMap = {}
    for await (const filename of allFiles(jsonDir, /\.json$/)) {
        if (!filename.endsWith('.json')) { continue; }
        const links = await genLinksFromJsonFile(filename);
        linksMap[filename] = links;
    }
    if (needMerge) {
        const mergeFile = outputDir + '/merge-all';
        linksMap[mergeFile] = [...new Set(Object.values(linksMap).flat(1))];
    }
    for (const filename in linksMap) {
        const graph = `digraph all {\n${linksMap[filename].join('\n')}\n}`;
        const outputfile = outputDir + '/' + filename.substring(filename.lastIndexOf('/') + 1);
        await mkdir(dirname(outputfile), { recursive: true });
        await writeFile(`${outputfile}.dot`, graph)
        if (exec(`dot ${outputfile}.dot -T png -o ${outputfile}.dot.png`).code !== 0) {
            console.error(`Error: generate include graph fail for ${outputfile}`)
        } else {
            console.log(`generate include graph in ${outputfile}.dot.png`)
        }
    }
}


async function main() {
    const options = program.opts();
    program.parse();
    await doMain(options.jsonDir, options.outputDir, options.withMerge);
}

main().then()