import load from './Loader.js';
import Canvas from "./Canvas.js";
import repl from 'node:repl';
import Plotter, { delay } from './Plotter.js';
import { writeFile } from "node:fs/promises";

/*const img = await load('anemomeme.svg');
writeFile('r.json', JSON.stringify(img));
const cv = new Canvas();
await cv.draw(img.img);*/

const pl = new Plotter(process.env.SERIAL?.trim(), process.env.BAUD?.trim());
await pl.ready.wait(20000);

process.on('SIGINT', () => {
    process.removeAllListeners('exit');
    pl.quit();
    process.exit();
});
process.on('exit', pl.quit.bind(pl));

const r = repl.start();
r.context.pl = pl;