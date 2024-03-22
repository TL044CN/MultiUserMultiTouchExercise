
import { OSCReceiver, Cursor2D } from "./oscReceiver";
import { EventEmitter, once } from "events";

export class OSCManager extends EventEmitter {

    private mReceiver: OSCReceiver

    private mCursors: Map<number,Cursor2D[]>;

    constructor() {
        super()
        this.mReceiver = new OSCReceiver('127.0.0.1', 3333);
        this.mCursors = new Map;

        once(this.mReceiver, 'ready').then(() => this.emit('ready'));

        this.mReceiver.on("setCursor", this.handleSetCursor.bind(this));
        this.mReceiver.on("aliveCursor", this.handleAliveCursor.bind(this));

    }

    private handleAliveCursor(ids: number[]): void {

        let finished: Map<number, Cursor2D[]> = new Map;

        const removedEntries = [...this.mCursors.entries()].filter(([key]) => !ids.includes(key));

        removedEntries.forEach(([key, value]) => {
            finished.set(key, value);
            this.mCursors.delete(key);
        });
/*        
        this.mCursors = new Map<number, Cursor2D[]>(
            [...this.mCursors.entries()].filter(([key]) => ids.includes(key))
        );
*/
        this.emit("update", this.mCursors, finished);
    }

    private handleSetCursor(cursor: Cursor2D): void {
        if(!this.mCursors.has(cursor.sessionID)){
            this.mCursors.set(cursor.sessionID,new Array<Cursor2D>)
        }

        (this.mCursors.get(cursor.sessionID) as Cursor2D[]).push(cursor)
        this.emit("setCursor", cursor);
    }

    get activeCursors(): Map<number,Cursor2D[]> {
        return this.mCursors;
    }

}
