import { Bundle, ArgumentType, Server } from 'node-osc'
import { EventEmitter } from 'events'

export interface Vec2 {
    X: number
    Y: number
}

export interface Cursor2D {
    sessionID: number
    position: Vec2
    velocity: Vec2
    accelleration: number
}

export type CursorListener = (cursor: Cursor2D) => void

/**
 * @event setCursor 
 */
export class OSCReceiver extends EventEmitter {

    private oscServer: Server;

    constructor(host: string, port: number) {
        super()
        this.oscServer = new Server(port, host, () => {
            this.emit('ready')
        })

        this.oscServer.on('bundle', this.handleBundle.bind(this))
    }

    private handleBundle(bundle: Bundle): void {
        for (const element of bundle.elements) {

            const message = ((element as unknown) as [string, ...ArgumentType[]]);

            if (message === undefined) continue;

            var eventName: string;
            switch (message[0]) {
                case '/tuio/2Dcur':
                    eventName = `${message[1] as string}Cursor`
                    let cursor: any;
                    switch (message[1]) {
                        case 'set':
                            cursor = <Cursor2D>{
                                sessionID: message[2] as number,
                                position: <Vec2>{ X: message[3] as number, Y: message[4] as number },
                                velocity: <Vec2>{ X: message[5] as number, Y: message[6] as number },
                                accelleration: message[7] as number
                            }

                            break;
                        case 'alive':
                            cursor = message.slice(2)
                        break;
                    }
                   this.emit(eventName, cursor)
                    break;                    
                }
        }

    }

}
