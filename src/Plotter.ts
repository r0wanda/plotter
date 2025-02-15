import _ora from 'ora';
import { SerialPort } from "serialport";
import { EventEmitter } from 'node:events';
//import { createWriteStream } from 'node:fs';
import symbols from 'log-symbols';
import logUpdate from 'log-update';
import chalk from 'chalk';
import { ReadlineParser as Readline } from '@serialport/parser-readline';

export const ora = (text: string) => _ora({ text, spinner: 'sand' }).start();

export class Signal<R extends any = void> extends EventEmitter {
    done: (r: R) => void;
    #done: boolean;
    #res!: R;
    constructor() {
        super();
        this.#done = false;
        this.done = ((r: R) => {
            this.#done = true;
            this.#res = r;
            this.emit('done', r);
        }).bind(this);
    }
    wait(timeout = -1): Promise<R> {
        return this.#done ? Promise.resolve<R>(this.#res) : new Promise<R>((res, j) => {
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
    ready: Signal;
    spin?: ReturnType<typeof ora>;
    //_log: ReturnType<typeof createWriteStream>;
    _prevLog?: string;
    _logRpt: number;
    constructor(serial?: string, baud?: number | string) {
        this.path = serial || '/dev/ttyACM0';
        this.baud = parseInt((<string>baud) || '9600');
        if (isNaN(this.baud)) this.baud = 9600;
        //this._log = createWriteStream('plotter.log');
        this._logRpt = 1;
        this.port = new SerialPort({
            path: this.path,
            baudRate: this.baud
        });
        this.ready = new Signal();
        if (this.port.readable) {
            this.ready.done();
        } else {
            this.port.once('readable', this.ready.done);
        }
        this.parse = this.port.pipe(new Readline({ delimiter: '\r\n' }));
        this.spin = <any>null;
        this.parse.on('data', (d: string) => {
            if (d.startsWith('E')) this.log(d.slice(2), 'error');
            if (d.startsWith('I')) {
                this.log(d.slice(2));
            }
        });
    }
    log(str: string, sym: keyof typeof symbols = 'info') {
        if (this.spin && this.spin.isSpinning) this.spin.clear();
        let nLog = `${symbols[sym] ?? symbols.info} ${str}`;
        if (nLog === this._prevLog) {
            this._logRpt++;
            nLog = `${nLog} ${chalk.yellow(`(x${this._logRpt})`)}`;
        } else {
            logUpdate.done();
            this._logRpt = 1;
            this._prevLog = nLog;
        }
        logUpdate(nLog);
        if (this.spin && this.spin.isSpinning) this.spin.render();
    }
    async home() {
        this.port.write('H\n')
        this.log('Homing to bottom');
        const sum = {
            bottom: 0,
            top: 0
        }
        const sig = new Signal<typeof sum>();
        this.spin = ora('Step: ');
        let handle = (d: any) => {
            //this._log.write(d);
            const s = d.trim().split(':');
            const cmd = s[0];
            const res = s[1];
            switch (cmd) {
                case 'HBOT':
                    this.log('Reached bottom, homing to top');
                    sum.bottom = res;
                    break;
                case 'HTOP':
                    this.spin?.succeed(`Reached top, total steps: ${res}`);
                    sum.top = res;
                    sig.done(sum);
                    break;
                case 'OK':
                    break;
                default:
                    if (cmd.startsWith('P')) this.spin!.text = `Axis: ${cmd.charAt(1)}, Speed: ${cmd.charAt(2)}, Dir: ${cmd.charAt(3)}, Step: ${res}`;
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
            this.log(d);
            if (d === 'ok') {
                sig.done(this.ok(--rep, time));
            }
        });
        return sig.wait(time);
    }
    async motor(motor: number, rep = 0) {
        let p = this.ok(++rep);
        this.port.write(`${motor}`);
        return await p;
    }
    quit() {
        this.log('✨ Gracefully ✨ exiting');
        this.parse.end();
        this.port.close();
    }
}