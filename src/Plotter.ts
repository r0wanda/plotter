import { SerialPort } from "serialport";
import { ReadlineParser as Readline } from '@serialport/parser-readline';

export default class Plotter {
    path: string;
    baud: number;
    port: SerialPort;
    parse: Readline;
    constructor(serial?: string) {
        this.path = serial || '/dev/ttyACM0';
        this.baud = parseInt(process.env.BAUD || '9600');
        if (isNaN(this.baud)) this.baud = 9600;
        this.port = new SerialPort({
            path: this.path,
            baudRate: this.baud
        });
        this.parse = this.port.pipe(new Readline({ delimiter: '\n' }));
    }
    ok(rep = 1, time = 2000): Promise<any> {
        if (rep < 1) {
            return Promise.resolve();
        }
        return new Promise<any>((r, j) => {
            const t = setTimeout(() => {
                console.error('Error: Exceeded timeout');
                j();
            }, time);
            this.parse.once('data', d => {
                clearTimeout(t);
                console.log(d);
                if (d === 'ok') {
                    r(this.ok(--rep, time));
                }
            });
        });
    }
    async motor(motor: number, rep = 0) {
        let p = this.ok(1 + rep);
        this.port.write(`m${motor}`);
        return await p;
    }
    async backward(motor: number) {
        let p = this.motor(motor, 1);
        this.port.write('b');
        await p;
    }
    async forward(motor: number) {
        let p = this.motor(motor, 1);
        this.port.write('f');
        await p;
    }
}