import open from "open";
import { createCanvas, type CanvasRenderingContext2D } from "canvas";
import { type LoadRes } from "./Loader.js";

export default class Canvas {
    canvas: ReturnType<typeof createCanvas>;
    ctx: CanvasRenderingContext2D;
    dims: [number, number];
    constructor(dims?: [number, number]) {
        this.dims = !dims || dims.length < 2 ? [278, 214] : dims;
        this.canvas = createCanvas(...this.dims);
        this.ctx = this.canvas.getContext('2d');
    }
    draw(img: LoadRes) {
        this.ctx.clearRect(0, 0, ...this.dims);
        this.ctx.strokeStyle = 'rgba(0,0,0,1)';
        for (const l of img) {
            this.ctx.beginPath();
            this.ctx.moveTo(...l[0]);
            for (let i = 1; i < l.length; i++) {
                const c = l[i];
                this.ctx.lineTo(...c);
                if (c[0] > this.dims[0] || c[1] > this.dims[1]) console.log(JSON.stringify(c));
            }
            this.ctx.stroke();
        }
        return open(this.canvas.toDataURL());
    }
}
