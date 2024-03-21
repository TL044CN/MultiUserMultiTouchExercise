import { Bundle, ArgumentType, Server } from 'node-osc'
import { EventEmitter } from 'events'

export interface vec2 {
    x: number
    y: number
}

export interface Cursor2D {
    sessionID: number
    position: vec2
    velocity: vec2
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
        for (var element of bundle.elements) {

            var message = ((element as unknown) as [string, ...ArgumentType[]]);

            if (message === undefined) continue;

            switch (message[0]) {
                case '/tuio/2Dcur':
                    var eventName: string = `${message[1] as string}Cursor`;
                    var cursor: any;
                    switch (message[1]) {
                        case 'set':
                            cursor = <Cursor2D>{
                                sessionID: message[2] as number,
                                position: <vec2>{ x: message[3] as number, y: message[4] as number },
                                velocity: <vec2>{ x: message[5] as number, y: message[6] as number },
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
