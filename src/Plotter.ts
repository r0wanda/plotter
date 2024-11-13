import _ora from 'ora';
import { SerialPort } from "serialport";
import { EventEmitter } from 'node:events';
import { ReadlineParser as Readline } from '@serialport/parser-readline';

export const ora = (text: string) => _ora({ text, spinner: 'sand' }).start();

export class Signal<R extends any = undefined> extends EventEmitter {
    done(r: R) {
        this.emit('done', r);
    }
    wait(timeout = -1): Promise<R> {
        return new Promise<R>((res, j) => {
            let t: NodeJS.Timeout | undefined = undefined;
            if (timeout > 0) t = setTimeout(j, timeout);
            this.once('done', (r: R) => {
                if (t !== undefined) clearTimeout(t);
                res(r);
            });
        });
    }
}
export function delay(ms: number) {
    return new Promise<void>(r => setTimeout(r, ms));
}

export default class Plotter {
    path: string;
    baud: number;
    port: SerialPort;
    parse: Readline;
    spin: ReturnType<typeof ora>;
    constructor(serial?: string, baud?: number | string) {
        this.path = serial || '/dev/ttyACM0';
        this.baud = parseInt((<string>baud) || '9600');
        if (isNaN(this.baud)) this.baud = 9600;
        this.port = new SerialPort({
            path: this.path,
            baudRate: this.baud
        });
        this.parse = this.port.pipe(new Readline({ delimiter: '\r\n' }));
        this.spin = <any>null;
        this.parse.on('data', (d: string) => {
            if (d.startsWith('E')) throw new Error(d.replace('E:', ''));
            if (d.startsWith('I')) console.log(d.replace('I:', 'Info: '));
        });
    }
    async home() {
        this.port.write('h')
        console.log('Homing to bottom');
        const sum = {
            bottom: 0,
            top: 0
        }
        const sig = new Signal<typeof sum>();
        this.spin = ora('Step: ');
        let handle = (d: any) => {
            //console.log(d);
            const s = d.trim().split(':');
            const cmd = s[0];
            const res = s[1];
            switch (cmd) {
                case 'BOT': 
                    console.log('Reached bottom, homing to top');
                    sum.bottom = res;
                    break;
                case 'TOP':
                    this.spin.succeed(`Reached top, total steps: ${res}`);
                    sum.top = res;
                    sig.done(sum);
                    break;
                default:
                    if (cmd.startsWith('P')) this.spin.text = `Step: ${res} (mode: ${cmd})`;
            }
        }
        this.parse.on('data', handle);
        return await sig.wait();
    }
    ok(rep = 1, time = 2000): Promise<any> {
        if (rep < 1) {
            return Promise.resolve();
        }
        const sig = new Signal<any>();
        this.parse.once('data', d => {
            console.log(d);
            if (d === 'ok') {
                sig.done(this.ok(--rep, time));
            }
        });
        return sig.wait(time);
    }
    async motor(motor: number, rep = 0) {
        let p = this.ok(++rep);
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
    quit() {
        console.log('✨ Gracefully ✨ exiting');
        this.parse.end();
        this.port.close();
    }
}