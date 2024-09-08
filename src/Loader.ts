import _ora from 'ora';
import { Readable } from 'node:stream';
import { convertMany as convert } from 'convert';
import { parse as svgParse, type ElementNode } from 'svg-parser';
import { spawn } from 'node:child_process';
import { readFile } from 'node:fs/promises';
import { createParseStream } from 'big-json';
import { join } from 'desm';

type LineKey = `Line${number}`;
interface Coords {
    X: number;
    Y: number;
}
interface VpypeJson {
    Layer: {
        [key: LineKey]: Coords[];
    }
}
export type LoadRes = [number, number][][];

export const ora = (text: string) => _ora({ text, spinner: 'sand'}).start();

/*function _bestSize(w1: number, h1: number, w2: number, h2: number) {
    let nW;
    let nH;
    if (w1 > h1) {
        nH = h1;
        nW = h1 * w2 / h2;
    } else {
        nW = w1;
        nH = w1 * h2 / w2;
    }
    if (nW > w1 || nH > h1) throw new Error('Image size exceeds page size');
    console.log([nW, nH]);

    return {
        w: nW,
        h: nH
    }
}
function bestSize(_svg: string, _width: string, _height: string) {
    const svg = svgParse(_svg);
    const svgNode = <ElementNode>svg.children.find(c => (<ElementNode>c).tagName && (<ElementNode>c).tagName === 'svg');
    let _w2 = svgNode.properties?.width;
    if (!_w2) throw new Error('Could not find svg width');
    _w2 = _w2.toString();
    let _h2 = svgNode.properties?.width;
    if (!_h2) throw new Error('Could not find svg height');
    _h2 = _h2.toString();

    const w1 = convert(_width).to('mm');
    const h1 = convert(_height).to('mm');
    const w2 = convert(_w2).to('mm');
    const h2 = convert(_h2).to('mm');
    const nR = _bestSize(w1, h1, w2, h2);
    const r = _bestSize(h1, w1, h2, w2);

    const a = [];
    let nW;
    let nH;
    if (nR.w * nR.h < r.w * r.h) {
        a.push('rotate', '270');
        nW = r.w;
        nH = r.h;
    } else {
        nW = nR.w;
        nH = nR.h;
    }
    //a.push('scaleto', '--origin', '0', '0', `${nW}mm`, `${nH}mm`);
    console.log(a)
    return a;
}*/

export default async function load(path: string, group = true, dedupe = true, dims?: [string, string], origin?: [number, number]) {
    if (!dims || dims.length < 2) dims = ['278mm', '214mm'];
    if (!origin || origin.length < 2) origin = [0, 0];
    const og = origin.map(v => v.toString());

    const vpS = Date.now();
    let spin = ora('Running vpype & parsing response');
    const file = await readFile(path, 'utf8');
    //const sz = bestSize(file, ...dims);
    const vpype = spawn('vpype', [
        '-c', join(import.meta.url, '..', 'vpype.toml'),
        'read', '-',
        'pagesize', '-l', dims.join('x'),
        /*...sz,*/
        'layout', '-l', '-m', '1cm', dims.join('x'),
        'gwrite', '-p', 'json_mod', '-'
    ], {
        stdio: 'pipe'
    });
    const parse = <NodeJS.WritableStream><unknown>createParseStream();
    const fStrm = new Readable();
    fStrm._read = () => {};
    fStrm.push(file);
    fStrm.push(null);
    fStrm.pipe(vpype.stdin);
    vpype.stdout.pipe(parse);
    const { Layer: raw } = await new Promise<VpypeJson>(r => {
        parse.on('data', r);
    });
    const ogSize = parseFloat((Buffer.byteLength(JSON.stringify(raw), 'utf8') / 1000).toFixed(1));
    const vpE = Date.now();
    spin.succeed(`Vpype + parsing done in ${vpE - vpS}ms`);
    spin.text = 'Processing data';
    spin.start();

    const p: LoadRes = [];
    const parseKey = (k: LineKey) => parseInt((/Line(\d+)/.exec(k) ?? [])[1]);
    const getGrpDir = (a: [number, number], b: [number, number]) => {
        const chX = a[0] === b[0];
        const chY = a[1] === b[1];
        return chX && chY ? -2 : (!chX && !chY ? -1 : (chX ? 0 : 1));
    }
    let total = 0;
    let dedupeL = 0;
    let grpL = 0;
    let grpC = 0;
    for (const k of (<LineKey[]>Object.keys(raw)).toSorted((a, b) => parseKey(a) - parseKey(b))) {
        let r = raw[k].map(c => <[number, number]>[c.X, c.Y]);
        total += r.length;
        let l: [number, number][] = [];
        if (dedupe || group) {
            let prev: number[] = [];
            for (let i = 0; i < r.length; i++) {
                if (r[i + 1]) {
                    const grpS = r[i];
                    const sI = i;
                    let grpDir: number = getGrpDir(grpS, r[i + 1]);
                    if (dedupe && grpDir === -2) {
                        dedupeL++;
                        continue;
                    }
                    if (group && grpDir >= 0) {
                        for (;r[i + 1] && getGrpDir(grpS, r[i + 1]) === grpDir; i++) { grpL++; };
                        grpC++;
                        if (i !== sI) l.push(grpS);
                    }
                }
                l.push(r[i]);
                prev = r[i];
            }
        } else l = r;
        p.push(l);
    }

    const finT = Date.now();
    const finL = total - dedupeL - grpL;
    const fSize = parseFloat((Buffer.byteLength(JSON.stringify(p), 'utf8') / 1000).toFixed(1));
    spin.succeed(`Processing done in ${finT - vpE}ms`);
    if (dedupe) spin.info(`Items removed from deduping: ${dedupeL}`);
    if (group) spin.info(`Items removed from grouping: ${grpL}`);
    spin.info(`Final length: ${finL} (${Math.round(100 - finL * 100 / total)}% reduction from ${total})`);
    spin.info(`Final size: ${fSize}kb (${Math.round(100 - fSize * 100 / ogSize)}% reduction from ${ogSize})`);
    spin.info(`Total lines: ${p.length}`);
    if (group) spin.info(`Total groups: ${grpC}`);
    spin.succeed(`Done in ${finT - vpS}ms`);

    return {
        img: p,
        svg: file
    };
}
