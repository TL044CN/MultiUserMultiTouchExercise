import { BrowserWindow, app } from 'electron';
import { once } from 'events';
import path from 'path';

import { OSCManager } from './oscManager';
import { Cursor2D } from './oscReceiver';

import { DollarRecognizer, Result as Gesture } from './vendor/oneDollar'
import { Point } from 'build/vendor/oneDollar';

const recognizer = new DollarRecognizer

async function init(): Promise<[OSCManager, any[]]> {
    return Promise.all([
        new Promise<OSCManager>((resolve) => {
            const manager = new OSCManager();
            manager.on('ready', () => { resolve(manager); })
        }),
        once(app, 'ready')
    ]);
}

async function main() {
    const [manager, _] = await init()

    const mainWindow = await new Promise<BrowserWindow>((resolve) => {
        const mainWindow = new BrowserWindow({ width: 1600, height: 900 })

        const indexHTML = path.join(`${__dirname}/resources/index.html`)
        mainWindow.loadFile(indexHTML).finally(() => resolve(mainWindow))

    })


    manager.on("update", (cursors: Map<number,Cursor2D[]>) => {
        const content = mainWindow.webContents;
        let tableData: string = "";

        let gestures: Gesture[] = new Array<Gesture>;

        for(const [_, cursorArray] of cursors) {
            const pointsArray: Point[] = cursorArray.map(cursor => ({ X: cursor.position.x, Y: cursor.position.y }));
            gestures.push(recognizer.Recognize(pointsArray))
        }

        for(const gesture of gestures) {
            tableData += `\
            <tr><td>${gesture.Name}</td><td>${gesture.Score}</td></tr>`
        }

const dynamicTableBody = `\
<table>\
    <thead>\
        <tr>\
            <th>Gesture</th>\
            <th>Score</th>\
        <tr>\
    </thead>\
    <tbody>\
        ${tableData}\
    </tbody>\
</table>\
`

        content.executeJavaScript(`\
document.getElementById('table_wrapper').innerHTML = '${dynamicTableBody}';
        `);

    })

}

main();
