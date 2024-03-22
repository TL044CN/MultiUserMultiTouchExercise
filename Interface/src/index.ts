import { BrowserWindow, app } from 'electron';
import { once } from 'events';
import path from 'path';

import { OSCManager } from './oscManager';
import { Cursor2D, Vec2 } from './oscReceiver';

import { BoundingBox, Centroid, DollarRecognizer, Result as Gesture } from './vendor/oneDollar'
import { Point, Rectangle } from 'build/vendor/oneDollar';
import { isIdentifier } from 'typescript';

const recognizer = new DollarRecognizer

interface MetaRect {
    rectangle: Rectangle
    age: number
    velocity: Vec2
}

let rects: Array<MetaRect> = new Array;

function isPointInsideRect(rect: Rectangle, point: Point): boolean {
    return point.X >= rect.X && point.X <= rect.X + rect.Width &&
           point.Y >= rect.Y && point.Y <= rect.Y + rect.Height;
}

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
        const mainWindow = new BrowserWindow({
            width: 1600, height: 900,
            webPreferences: {
                preload: path.join(__dirname, "preload.js")
            }
        })

        const indexHTML = path.join(`${__dirname}/resources/index.html`)
        mainWindow.loadFile(indexHTML).finally(() => resolve(mainWindow))

    })

    manager.on("update", (inProgress: Map<number, Cursor2D[]>, finished: Map<number, Cursor2D[]>) => {
        const content = mainWindow.webContents;
        let tableData: string = "";

        let gestures: Array<[Gesture, Point[]]> = new Array;

        for (const [_, cursorArray] of finished) {
            const pointsArray: Point[] = cursorArray.map(cursor => ({ X: cursor.position.X, Y: cursor.position.Y }));
            const gesture: Gesture = recognizer.Recognize(pointsArray);
            gestures.push([gesture, pointsArray]);

        }

        for (const [gesture, points] of gestures) {

            if (gesture.Name === "rectangle") {

                rects.push({
                    rectangle: BoundingBox(points),
                    age: 0,
                    velocity: {X: 0, Y: 0}
                });

            }

            if (gesture.Name === "delete") {
                console.log("delete")
                const center = Centroid(points);
                for(const [index, metaRect] of rects.entries()){
                    console.log("check...",index)
                    console.log(center)
                    if(isPointInsideRect(metaRect.rectangle, center)){
                        rects.splice(index,1);
                        console.log("deleting...")
                    }
                }
            }

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

    manager.on("setCursor", (cursor: Cursor2D) => {
        for(const [index, metaRect] of rects.entries()) {
            if(isPointInsideRect(metaRect.rectangle, cursor.position)) {
                rects[index].age = 0;
                rects[index].velocity.X += cursor.velocity.X;
                rects[index].velocity.Y += cursor.velocity.Y;
            }
        }
    });

    setInterval(() => {
        mainWindow.webContents.send('canvas-clear');
        for (const [index, rect] of rects.entries()) {
            if (rect.age >= 1000) {
                rects.splice(index, 1);
                continue;
            }

            const {X,Y} = rects[index].velocity;

            rects[index].velocity = {X: X*0.5-X*0.1, Y: Y*0.5-Y*0.1}

            const box = rect.rectangle
            rects[index].age++;
            rects[index].rectangle.X += (rect.velocity.X) * 0.05
            rects[index].rectangle.Y += (rect.velocity.Y) * 0.05
            mainWindow.webContents.send('canvas-manipulation', { action: 'drawRectangle', position: box, width: box.Width, height: box.Height });
        }
    }, 10);

}

main();
