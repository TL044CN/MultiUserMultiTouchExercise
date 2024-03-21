
import { OSCReceiver, Cursor2D } from "./oscReceiver";
import { EventEmitter, once } from "events";

export class OSCManager extends EventEmitter {

    private receiver: OSCReceiver

    constructor() {
        super()
        this.receiver = new OSCReceiver('127.0.0.1', 3333);
        this.receiver.on('cursor', this.handleSetCursor);
        
        once(this.receiver, 'ready').then(() => this.emit('ready'));

//        this.receiver.on('setCursor', this.handleSetCursor)

    }

    private handleSetCursor(cursor: Cursor2D) {
        console.log(cursor);
    }
}
