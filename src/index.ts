import load from './Loader.js';
import Canvas from "./Canvas.js";
import Plotter, { delay } from './Plotter.js';
import { writeFile } from "node:fs/promises";

/*const img = await load('anemomeme.svg');
writeFile('r.json', JSON.stringify(img));
const cv = new Canvas();
await cv.draw(img.img);*/

const pl = new Plotter(process.env.SERIAL?.trim(), process.env.BAUD?.trim());

process.on('SIGINT', () => {
    process.removeAllListeners('exit');
    pl.quit();
    process.exit();
});
process.on('exit', pl.quit.bind(pl));

console.log(await pl.home());
delay(500);
console.log(await pl.home());
delay(500);
console.log(await pl.home());