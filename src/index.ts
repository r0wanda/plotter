import load from './Loader.js';
import Canvas from "./Canvas.js";
import Plotter from './Plotter.js';
import { writeFile } from "node:fs/promises";

/*const img = await load('anemomeme.svg');
writeFile('r.json', JSON.stringify(img));
const cv = new Canvas();
await cv.draw(img.img);*/

const pl = new Plotter(process.env.SERIAL?.trim());

setTimeout(() => {
    pl.backward(1);
}, 1000);
setTimeout(() => {
    pl.forward(1);
}, 2000);
