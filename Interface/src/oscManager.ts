
import { OSCReceiver, Cursor2D } from "./oscReceiver";
import { EventEmitter, once } from "events";

export class OSCManager extends EventEmitter {

    private mReceiver: OSCReceiver

    private mCursors: Cursor2D[];

    constructor() {
        super()
        this.mReceiver = new OSCReceiver('127.0.0.1', 3333);
        this.mCursors = [];

        once(this.mReceiver, 'ready').then(() => this.emit('ready'));

        this.mReceiver.on("setCursor", this.handleSetCursor.bind(this));
        this.mReceiver.on("aliveCursor", this.handleAliveCursor.bind(this));

    }

    private handleAliveCursor(ids: number[]): void {
//        this.mCursors.filter(cursor => ids.includes(cursor.sessionID));
        this.emit("update", this.mCursors);
        this.mCursors = [];
    }

    private handleSetCursor(cursor: Cursor2D): void {
        this.mCursors.push(cursor);
    }

    get activeCursors(): Cursor2D[] {
        return this.mCursors;
    }

}
